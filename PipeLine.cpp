#include "PipeLine.h"

void CPipeLine::InitZBuffer(int width, int height) {
	m_Width = width;
	m_Height = height;
	m_ZBuffer.resize(width * height, 1.0f);
}

void CPipeLine::ClearZBuffer() {
	std::fill(m_ZBuffer.begin(), m_ZBuffer.end(), 1.0f);
}

void CPipeLine::DrawFaceZBuffer(HDC hDC, CVertex v1, CVertex v2, CVertex v3, COLORREF color) {
	// 삼각형을 포함하는 최소한의 바운딩 박스를 구합니다.
	int minX = (std::max)(0, (int)(std::min)({ v1.v.x, v2.v.x, v3.v.x }));
	int maxX = (std::min)(m_Width - 1, (int)(std::max)({ v1.v.x, v2.v.x, v3.v.x }));
	int minY = (std::max)(0, (int)(std::min)({ v1.v.y, v2.v.y, v3.v.y }));
	int maxY = (std::min)(m_Height - 1, (int)(std::max)({ v1.v.y, v2.v.y, v3.v.y }));

	// 삼각형의 전체 넓이 계산
	float area = (v2.v.x - v1.v.x) * (v3.v.y - v1.v.y) - (v3.v.x - v1.v.x) * (v2.v.y - v1.v.y);
	if (fabs(area) < 0.0001f) return; // 선분 형태의 삼각형은 무시

	// 나눗셈을 한 번만 수행하기 위해 역수를 구함
	float invArea = 1.0f / area;

	// X, Y 증가에 따른 w0, w1, w2의 변화량을 계산
	float dw0_dx = (v2.v.y - v3.v.y) * invArea;
	float dw0_dy = (v3.v.x - v2.v.x) * invArea;
	float dw1_dx = (v3.v.y - v1.v.y) * invArea;
	float dw1_dy = (v1.v.x - v3.v.x) * invArea;
	float dw2_dx = (v1.v.y - v2.v.y) * invArea;
	float dw2_dy = (v2.v.x - v1.v.x) * invArea;

	// 시작 픽셀에서의 초기 가중치 계산
	float px_start = (float)minX + 0.5f;
	float py_start = (float)minY + 0.5f;

	float w0_row = ((v2.v.x - px_start) * (v3.v.y - py_start) - (v3.v.x - px_start) * (v2.v.y - py_start)) * invArea;
	float w1_row = ((v3.v.x - px_start) * (v1.v.y - py_start) - (v1.v.x - px_start) * (v3.v.y - py_start)) * invArea;
	float w2_row = 1.0f - w0_row - w1_row;

	// COLORREF 포맷을 여기서 단 한 번만 DWORD 형태로 변환하여 화면에 그릴 색상을 캐싱
	DWORD finalColor = 0;
	if (m_pPixelBuffer != nullptr) {
		BYTE r = GetRValue(color);
		BYTE g = GetGValue(color);
		BYTE b = GetBValue(color);
		finalColor = (r << 16) | (g << 8) | b;
	}

	// 바운딩 박스 순회
	float invZ1 = 1.0f / v1.v.z;
	float invZ2 = 1.0f / v2.v.z;
	float invZ3 = 1.0f / v3.v.z;

	for (int y = minY; y <= maxY; ++y) {
		float w0 = w0_row, w1 = w1_row, w2 = w2_row;
		int rowOffset = y * m_Width;

		for (int x = minX; x <= maxX; ++x) {
			if (w0 >= 0.0f && w1 >= 0.0f && w2 >= 0.0f) {
				// 원근 보정 보간: 1/Z 값을 선형 보간함
				float interpolatedInvZ = (w0 * invZ1) + (w1 * invZ2) + (w2 * invZ3);
				float currentZ = 1.0f / interpolatedInvZ;

				int pixelIndex = rowOffset + x;

				if (currentZ < m_ZBuffer[pixelIndex]) {
					m_ZBuffer[pixelIndex] = currentZ;

					if (m_pPixelBuffer != nullptr) {
						m_pPixelBuffer[pixelIndex] = finalColor;
					}
				}
			}
			w0 += dw0_dx; w1 += dw1_dx; w2 += dw2_dx;
		}
		w0_row += dw0_dy; w1_row += dw1_dy; w2_row += dw2_dy;
	}
}

void CPipeLine::DrawObject(HDC hDC, CMesh* obj, COLORREF color) {
	XMMATRIX worldview = XMLoadFloat4x4(&mWorld) * XMLoadFloat4x4(&mView);
	for (int i = 0; i < obj->IndicesArray.size(); i += 3) {
		CFace face = CFace(
			obj->VerticesArray[obj->IndicesArray[i]],
			obj->VerticesArray[obj->IndicesArray[i + 1]],
			obj->VerticesArray[obj->IndicesArray[i + 2]]
		);
		XMVECTOR v[3] = {
			XMLoadFloat3(&face.Vertex[0].v),
			XMLoadFloat3(&face.Vertex[1].v),
			XMLoadFloat3(&face.Vertex[2].v)
		};

		// 월드 & 뷰 변환
		v[0] = XMVector3TransformCoord(v[0], worldview);
		v[1] = XMVector3TransformCoord(v[1], worldview);
		v[2] = XMVector3TransformCoord(v[2], worldview);

		// Near Plane Clipping (Sutherland-Hodgman 방식 기반)
		// 잘려진 다각형의 정점을 담을 임시 버퍼
		XMVECTOR clippedPoly[4];
		int polyCount = 0;
		float near_z;
		near_z = -mProj._43 / mProj._33;

		for (int j = 0; j < 3; j++) {
			int next = (j + 1) % 3;
			float d1 = XMVectorGetZ(v[j]) - near_z;
			float d2 = XMVectorGetZ(v[next]) - near_z;

			// 현재 정점이 Near Plane 앞에 있으면 추가
			if (d1 >= 0.0f) clippedPoly[polyCount++] = v[j];

			// 선분이 Near Plane을 교차하면 교차점 계산 후 추가
			if ((d1 >= 0.0f && d2 < 0.0f) || (d1 < 0.0f && d2 >= 0.0f)) {
				float t = d1 / (d1 - d2);
				clippedPoly[polyCount++] = XMVectorLerp(v[j], v[next], t);
			}
		}

		// 정점이 3개 미만이면 완전히 카메라 뒤로 넘어간 것이므로 그리지 않음
		if (polyCount < 3) continue;

		// 다각형(삼각형 또는 사각형)을 다시 삼각형으로 분할
		int triangleCount = (polyCount == 3) ? 1 : 2;
		XMVECTOR outTriangles[2][3];

		outTriangles[0][0] = clippedPoly[0];
		outTriangles[0][1] = clippedPoly[1];
		outTriangles[0][2] = clippedPoly[2];

		if (triangleCount == 2) {
			outTriangles[1][0] = clippedPoly[0];
			outTriangles[1][1] = clippedPoly[2];
			outTriangles[1][2] = clippedPoly[3];
		}

		// 클리핑되어 생성된 1개 또는 2개의 삼각형을 각각 처리
		for (int tIdx = 0; tIdx < triangleCount; ++tIdx) {
			XMVECTOR tv1 = outTriangles[tIdx][0];
			XMVECTOR tv2 = outTriangles[tIdx][1];
			XMVECTOR tv3 = outTriangles[tIdx][2];

			// 백페이스 컬링
			XMVECTOR edge1 = tv2 - tv1;
			XMVECTOR edge2 = tv3 - tv1;
			XMVECTOR faceNormalVec = XMVector3Cross(edge1, edge2);
			faceNormalVec = XMVector3Normalize(faceNormalVec);
			if (XMVectorGetX(XMVector3Dot(faceNormalVec, tv1)) >= 0) continue;

			// 투영 변환
			XMMATRIX Proj = XMLoadFloat4x4(&mProj);
			tv1 = XMVector3Transform(tv1, Proj);
			tv2 = XMVector3Transform(tv2, Proj);
			tv3 = XMVector3Transform(tv3, Proj);

			// 원근 나누기
			float w0 = XMVectorGetW(tv1);
			float w1 = XMVectorGetW(tv2);
			float w2 = XMVectorGetW(tv3);
			if (fabs(w0) > 0.000000001f) tv1 = XMVectorDivide(tv1, XMVectorSplatW(tv1));
			if (fabs(w1) > 0.000000001f) tv2 = XMVectorDivide(tv2, XMVectorSplatW(tv2));
			if (fabs(w2) > 0.000000001f) tv3 = XMVectorDivide(tv3, XMVectorSplatW(tv3));

			// 뷰포트 변환
			XMMATRIX viewport = XMLoadFloat4x4(&mViewport);
			tv1 = XMVector3TransformCoord(tv1, viewport);
			tv2 = XMVector3TransformCoord(tv2, viewport);
			tv3 = XMVector3TransformCoord(tv3, viewport);

			CVertex cv1, cv2, cv3;
			XMStoreFloat3(&cv1.v, tv1);
			XMStoreFloat3(&cv2.v, tv2);
			XMStoreFloat3(&cv3.v, tv3);

			// 그리기
			DrawFaceZBuffer(hDC, cv1, cv2, cv3, color);
		}
	}
}