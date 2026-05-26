#include "Enemy.h"

CEnemy::CEnemy(std::shared_ptr<CMesh> pMesh, std::shared_ptr<CShader> pShader,
	XMFLOAT3 position, XMFLOAT3 rotation, ObjectType type) : CGameObject(pMesh, pShader, position, rotation, type) {
}

void CEnemy::Animate(float time) {
	Rotation.y += 30.f * time; // 초당 30도 회전
	SetWorldMatrix();
}