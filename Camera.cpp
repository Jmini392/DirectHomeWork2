#include "Camera.h"

CCamera::CCamera() {
	m_Viewport = std::make_unique<CViewport>();
	SetViewMatrix();
	SetProjMatrix();
	SetViewport();
	SetScissorRect();
}

void CCamera::SetViewMatrix() {
	// 카메라의 회전(Rotation) 각도를 라디안으로 변환
	float pitch = XMConvertToRadians(Rotation.x);
	float yaw = XMConvertToRadians(Rotation.y);

	// 구면 좌표계 공식을 이용하여 회전된 LOOK 벡터 계산
	float fLookX = sinf(yaw) * cosf(pitch);
	float fLookY = sinf(pitch);
	float fLookZ = cosf(yaw) * cosf(pitch);
	
	if (isFirstPersonView) AT = XMFLOAT3(EYE.x + fLookX, EYE.y + fLookY, EYE.z + fLookZ);
	else AT = TargetPos;

	// Look을 AT과 EYE의 차이로 설정
	LOOK = Vector3::Subtract(AT, EYE);

	UP = XMFLOAT3(0.0f, 1.0f, 0.0f);

	// Right를 Look과 UP의 외적으로 설정
	RIGHT = Vector3::CrossProduct(UP, LOOK);

	// UP을 Right와 Look의 외적으로 설정
	UP = Vector3::CrossProduct(LOOK, RIGHT);

	// Look, Right, UP 벡터를 정규화
	LOOK = Vector3::Normalize(LOOK);
	RIGHT = Vector3::Normalize(RIGHT);
	UP = Vector3::Normalize(UP);

	// LookAtLH 함수를 사용하여 ViewMatrix 설정
	ViewMatrix = Matrix4x4::LookAtLH(EYE, AT, UP);
}

void CCamera::SetProjMatrix() {
	// 원근 투영 변환 행렬을 생성한다.
	XMMATRIX mProj = XMMatrixPerspectiveFovLH(XMConvertToRadians(m_Viewport->Fov), m_Viewport->Aspect, m_Viewport->Near, m_Viewport->Far);

	ProjectionMatrix = Matrix4x4::Identity();
	XMStoreFloat4x4(&ProjectionMatrix, mProj);
}

void CCamera::SetViewport() {
	Viewport.TopLeftX = m_Viewport->ViewportX;
	Viewport.TopLeftY = m_Viewport->ViewportY;
	Viewport.Width = m_Viewport->ViewportWidth;
	Viewport.Height = m_Viewport->ViewportHeight;
	Viewport.MinDepth = m_Viewport->ViewportMinDepth;
	Viewport.MaxDepth = m_Viewport->ViewportMaxDepth;
}

void CCamera::SetScissorRect() {
	ScissorRect.left = m_Viewport->ViewportX;
	ScissorRect.top = m_Viewport->ViewportY;
	ScissorRect.right = m_Viewport->ViewportX + m_Viewport->ViewportWidth;
	ScissorRect.bottom = m_Viewport->ViewportY + m_Viewport->ViewportHeight;
}

void CCamera::VindingMatrix(ID3D12GraphicsCommandList* pd3dCommandList) {
	XMFLOAT4X4 xmf4x4View;
	XMStoreFloat4x4(&xmf4x4View, XMMatrixTranspose(XMLoadFloat4x4(&ViewMatrix)));
	pd3dCommandList->SetGraphicsRoot32BitConstants(1, 16, &xmf4x4View, 0);

	XMFLOAT4X4 xmf4x4Projection;
	XMStoreFloat4x4(&xmf4x4Projection, XMMatrixTranspose(XMLoadFloat4x4(&ProjectionMatrix)));
	pd3dCommandList->SetGraphicsRoot32BitConstants(1, 16, &xmf4x4Projection, 16);
}

void CCamera::RSSetup(ID3D12GraphicsCommandList* pd3dCommandList) {
	pd3dCommandList->RSSetViewports(1, &Viewport);
	pd3dCommandList->RSSetScissorRects(1, &ScissorRect);
}