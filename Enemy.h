#pragma once
#include "PCH.h"
#include "GameObject.h"

class CEnemy : public CGameObject {
public:
	CEnemy() {}
	CEnemy(std::shared_ptr<CMesh> pMesh, std::shared_ptr<CShader> pShader,
		XMFLOAT3 position, XMFLOAT3 rotation, ObjectType type);
	~CEnemy() {}

	virtual void Animate(float time) override;
};