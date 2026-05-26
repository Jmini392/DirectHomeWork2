#include "Enemy.h"

CEnemy::CEnemy(std::shared_ptr<CMesh> pMesh, std::shared_ptr<CShader> pShader,
	XMFLOAT3 position, XMFLOAT3 rotation, ObjectType type, std::shared_ptr<CGameObject> pTarget) 
	: CGameObject(pMesh, pShader, position, rotation, type), m_pTarget(std::move(pTarget)) {
}

void CEnemy::Animate(float time) {
	bool isChasing = false;
	if (m_pTarget && !m_pTarget->isdead) {
		XMVECTOR enemyPos = XMLoadFloat3(&Position);
		XMFLOAT3 targetPt = m_pTarget->GetWorldBoundingBox().Center;
		XMVECTOR targetPos = XMLoadFloat3(&targetPt);

		XMVECTOR dir = XMVectorSubtract(targetPos, enemyPos);
		XMVECTOR lengthSqVec = XMVector3LengthSq(dir);
		float distSq = XMVectorGetX(lengthSqVec);

		if (distSq < m_DetectRange * m_DetectRange) {
			XMVECTOR dirNorm = XMVector3Normalize(dir);
			enemyPos = XMVectorAdd(enemyPos, XMVectorScale(dirNorm, m_Speed * time));
			
			float newY = Position.y;
			XMStoreFloat3(&Position, enemyPos);
			Position.y = newY;

			float dx = XMVectorGetX(dirNorm);
			float dz = XMVectorGetZ(dirNorm);
			Rotation.y = XMConvertToDegrees(atan2(dx, dz));

			isChasing = true;
		}
	}

	if (isChasing) SetColor(XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f)); // 쫒아올 때는 빨간색
	else SetColor(XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f)); // 평소엔 초록색

	SetWorldMatrix();
}

void CEnemy::OnCollision(std::shared_ptr<CGameObject> pOther) {
	if (pOther->GetType() == ObjectType::BULLET) {
		isdead = true;
	}
	else if (pOther->GetType() == ObjectType::PLAYER) {
		pOther->isdead = true;
	}
}