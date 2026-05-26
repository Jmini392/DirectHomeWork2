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

	void UpdateLocation();
	void Animate(float time) override;

	virtual void OnCollision(std::shared_ptr<CGameObject> pOther) override;

	void TransPersonView();
	void SetBullet(std::shared_ptr<CMesh> pMesh, std::shared_ptr<CShader> pShader);
	void Fire();
	void Dash(float speed);

	void AddChild(std::shared_ptr<CGameObject> pChild);
	std::vector<std::shared_ptr<CGameObject>>& GetChildren() { return m_Children; }
	virtual void Draw(ID3D12GraphicsCommandList* CommandList);
protected:
	std::vector<std::shared_ptr<CGameObject>> m_Children;
private:
	CCamera* m_pCamera = nullptr; // 플레이어의 카메라
	float cameraDistance = 5.f; // 카메라와 플레이어 사이의 거리

	bool isfalling = false; // 낙하 여부
	float fallSpeed = 1.f; // 낙하 속도
	XMFLOAT3 direction = { 0.0f, 0.0f, 1.0f }; // 플레이어가 바라보는 방향
	XMFLOAT3 m_PrevPosition = { 0.f, 0.f, 0.f }; // 충돌 시 위치 복구를 위한 이전 위치 저장
	float MoveSpeed = 0.1f; // 이동 속도

	bool isFirstPersonView = true; // 1인칭 시점 여부

	std::shared_ptr<CMesh> m_BulletMesh;
	std::shared_ptr<CShader> m_BulletShader;
};