#pragma once
#include "PCH.h"

class CViewport {
public:
	CViewport() {}
	virtual ~CViewport() {}

	float Fov = 90.0f; // 시야각(Field of View)
	float Near = 0.1f; // 근평면(Near Plane)
	float Far = 5000.0f; // 원평면(Far Plane)
	float Aspect = (float)FRAME_BUFFER_WIDTH / (float)FRAME_BUFFER_HEIGHT; // 종횡비(Aspect Ratio)

	float ViewportX = 0.f; // 뷰포트의 X 위치
	float ViewportY = 0.f; // 뷰포트의 Y 위치
	float ViewportWidth = (float)FRAME_BUFFER_WIDTH; // 뷰포트의 너비
	float ViewportHeight = (float)FRAME_BUFFER_HEIGHT; // 뷰포트의 높이
	float ViewportMinDepth = 0.0f; // 뷰포트의 최소 깊이
	float ViewportMaxDepth = 1.0f; // 뷰포트의 최대 깊이
};

class CCamera {
public:
	CCamera();
	virtual ~CCamera() {}

	void SetPosition(float x, float y, float z) { EYE = XMFLOAT3(x, y, z); }
	void SetRotation(float x, float y, float z) { Rotation = XMFLOAT3(x, y, z); }
	void SetPersonView(bool isFirstPerson) { isFirstPersonView = isFirstPerson; }
	void SetTarget(float x, float y, float z) { TargetPos = XMFLOAT3(x, y, z); } 
	
	void SetViewMatrix();
	void SetProjMatrix();
	void SetViewport();
	void SetScissorRect();

	void VindingMatrix(ID3D12GraphicsCommandList* pd3dCommandList);
	void RSSetup(ID3D12GraphicsCommandList* pd3dCommandList);
private:
	XMFLOAT3 EYE = { 0.f, 0.f, 0.f }; // 카메라의 위치
	XMFLOAT3 TargetPos = { 0.f, 0.f, 0.f }; // 카메라가 바라볼 대상의 위치
	XMFLOAT3 Rotation = { 0.f, 0.f, 0.f }; // 카메라의 회전

	XMFLOAT3 AT = {0.f, 0.f, 0.f}; // 카메라의 시선 방향 위치 벡터
	XMFLOAT3 UP = { 0.f, 1.f, 0.f }; // 카메라의 상향 벡터
	XMFLOAT3 LOOK = { 0.f, 0.f, 1.f }; // 카메라의 시선 방향 벡터	
	XMFLOAT3 RIGHT = { 1.f, 0.f, 0.f }; // 카메라의 오른쪽 방향 벡터
	
	XMFLOAT4X4 ViewMatrix = Matrix4x4::Identity(); // 카메라 행렬
	XMFLOAT4X4 ProjectionMatrix = Matrix4x4::Identity(); // 원근 투영 행렬
	D3D12_VIEWPORT Viewport; // 뷰포트
	D3D12_RECT ScissorRect; // 시저 렉트

	std::unique_ptr<CViewport> m_Viewport = nullptr; // 뷰포트 정보

	bool isFirstPersonView = true; // 1인칭 시점 여부
};