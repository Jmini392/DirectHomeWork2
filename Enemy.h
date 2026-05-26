#pragma once
#include "PCH.h"
#include "GameObject.h"

class CEnemy : public CGameObject {
public:
	CEnemy() {}
	CEnemy(std::shared_ptr<CMesh> pMesh, std::shared_ptr<CShader> pShader,
		XMFLOAT3 position, XMFLOAT3 rotation, ObjectType type, std::shared_ptr<CGameObject> pTarget = nullptr);
	~CEnemy() {}

	virtual void Animate(float time) override;

	virtual void OnCollision(std::shared_ptr<CGameObject> pOther) override;
private:
	std::shared_ptr<CGameObject> m_pTarget;
	float m_Speed = 7.0f;
	float m_DetectRange = 10.0f;
};