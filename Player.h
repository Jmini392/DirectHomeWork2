#pragma once
#include "PCH.h"
#include "GameObject.h"
#include "Camera.h"

class CScene;

class CPlayer : public CGameObject {
public:
	CPlayer() {}
	CPlayer(std::shared_ptr<CMesh> pMesh, std::shared_ptr<CShader> pShader,
		XMFLOAT3 position, XMFLOAT3 rotation, CCamera* camera);
	~CPlayer() {}

	void SetPosition(float x, float y, float z);
	void SetRotation(float x, float y, float z);
	void Move(int dir);
	void Rotate(float x, float y, float z);

	virtual void OnCollision(std::shared_ptr<CGameObject> pOther) override;

	void TransPersonView();

	void AddChild(std::shared_ptr<CGameObject> pChild);
	virtual void Draw(ID3D12GraphicsCommandList* CommandList);
protected:
	// 플레이어에게 부착된 자식 오브젝트들을 저장하는 리스트
	std::vector<std::shared_ptr<CGameObject>> m_Children;
private:
	CCamera* m_pCamera = nullptr; // 플레이어의 카메라
	
	XMFLOAT3 direction = { 0.0f, 0.0f, 1.0f }; // 플레이어가 바라보는 방향
	XMFLOAT3 m_PrevPosition = { 0.f, 0.f, 0.f }; // 충돌 시 위치 복구를 위한 이전 위치 저장
	float MoveSpeed = 0.5f; // 이동 속도

	bool isFirstPersonView = true; // 1인칭 시점 여부
};