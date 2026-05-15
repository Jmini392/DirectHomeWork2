#pragma once
#include "PCH.h"
#include "Mesh.h"

class CPipeLine {
public:
	CPipeLine() {}
	virtual ~CPipeLine() {}

	void InitZBuffer(int width, int height);
	void ClearZBuffer();

	void DrawObject(HDC hDC, CMesh* obj, COLORREF color);
	void DrawFaceZBuffer(HDC hDC, CVertex v1, CVertex v2, CVertex v3, COLORREF color);

	void SetWorldMatrix(const XMFLOAT4X4& world) { mWorld = world; }
	void SetViewMatrix(const XMFLOAT4X4& view) { mView = view; }
	void SetProjMatrix(const XMFLOAT4X4& proj) { mProj = proj; }
	void SetViewportMatrix(const XMFLOAT4X4& viewport) { mViewport = viewport; }

	void SetPixelBuffer(DWORD* pBuffer) { m_pPixelBuffer = pBuffer; }
private:
	XMFLOAT4X4 mWorld = Matrix4x4::Identity(); // 월드 변환 행렬
	XMFLOAT4X4 mView = Matrix4x4::Identity(); // 뷰 변환 행렬
	XMFLOAT4X4 mProj = Matrix4x4::Identity(); // 원근 투영 변환 행렬
	XMFLOAT4X4 mViewport = Matrix4x4::Identity(); // 화면 좌표 변환 행렬

	std::vector<float> m_ZBuffer;
	int m_Width = 0;
	int m_Height = 0;

	DWORD* m_pPixelBuffer = nullptr;
};