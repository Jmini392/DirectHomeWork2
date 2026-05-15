#include "Enemy.h"

CEnemy::CEnemy(int num) {
	SetType(ObjectType::ENEMY);
	std::shared_ptr<CMesh> pEnemyMesh;
	if (num == 0) {
		pEnemyMesh = std::make_shared<CCubeMesh>(4.f, 4.f, 4.f);
		SetColor(RGB(0, 0, 255));
		m_Speed = 17.0f;
		health = 1;
	}
	else if (num == 1) {
		pEnemyMesh = std::make_shared<CCubeMesh>(5.f, 5.f, 5.f);
		SetColor(RGB(255, 0, 0));
		m_Speed = 15.0f;
		health = 2;
	}
	else if (num == 2) {
		pEnemyMesh = std::make_shared<CCubeMesh>(6.f, 6.f, 6.f);
		SetColor(RGB(0, 255, 0));
		m_Speed = 13.0f;
		health = 3;
	}
	SetMesh(pEnemyMesh);
}

void CEnemy::Animate(float time) {
	// 적과 타겟의 위치 가져오기
	XMFLOAT3 currentPos = GetPosition();
	XMFLOAT3 targetPos = m_pTarget.lock()->GetPosition();

	// 방향 벡터 계산
	XMVECTOR vCurrent = XMLoadFloat3(&currentPos);
	XMVECTOR vTarget = XMLoadFloat3(&targetPos);
	XMVECTOR vDir = XMVectorSubtract(vTarget, vCurrent);

	// 거리가 너무 가까우면 이동 정지
	float distance = XMVectorGetX(XMVector3Length(vDir));
	if (distance > 0.5f) {
		vDir = XMVector3Normalize(vDir);
		
		float moveDist = m_Speed * time;
		vDir = XMVectorScale(vDir, moveDist);
		
		XMFLOAT3 moveDelta;
		XMStoreFloat3(&moveDelta, vDir);

		// 기존 위치에 더하기
		Move(moveDelta.x, 0.f, moveDelta.z);
	}
}

void CEnemy::OnCollision(std::shared_ptr<CGameObject> pOther) {
	// 총알과 충돌 시 소멸 처리
	if (pOther->GetType() == ObjectType::BULLET) {
		health--;
		if (health <= 0) isdead = true;
		else {
			if (health == 2) {
				SetMesh(std::make_shared<CCubeMesh>(5.f, 5.f, 5.f));
				SetColor(RGB(255, 0, 0));
				m_Speed = 15.0f;
			}
			else if (health == 1) {
				SetMesh(std::make_shared<CCubeMesh>(4.f, 4.f, 4.f));
				SetColor(RGB(0, 0, 255));
				m_Speed = 17.0f;
			}
		}
	}
}