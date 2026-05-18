#pragma once
#include "PCH.h"
#include "GameObject.h"
#include "Camera.h"

class CScene;

class CPlayer : public CGameObject {
public:
	CPlayer(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	virtual ~CPlayer() {}

	void SetCamera(CCamera* pCamera) { m_pCamera = pCamera; }

	void SetPosition(float x, float y, float z);
	void SetRotation(float x, float y, float z);
	void Move(int dir);
	void Rotate(float x, float y, float z);

	virtual void OnCollision(std::shared_ptr<CGameObject> pOther) override;
private:
	CCamera* m_pCamera = nullptr; // 플레이어의 카메라
	XMFLOAT3 direction = { 0.0f, 0.0f, 1.0f }; // 플레이어가 바라보는 방향
	XMFLOAT3 m_PrevPosition = { 0.f, 0.f, 0.f }; // 충돌 시 위치 복구를 위한 이전 위치 저장
	float MoveSpeed = 0.5f; // 이동 속도	
};