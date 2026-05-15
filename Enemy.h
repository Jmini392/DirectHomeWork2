#pragma once
#include "PCH.h"
#include "GameObject.h"

class CEnemy : public CGameObject {
public:
	CEnemy() {}
	CEnemy(int num);
	virtual ~CEnemy() {}
	virtual void Animate(float time) override;
	virtual void OnCollision(std::shared_ptr<CGameObject> pOther) override;

	// 추적할 대상 지정 함수
	void SetTarget(std::shared_ptr<CGameObject> pTarget) { m_pTarget = pTarget; }
private:
	std::weak_ptr<CGameObject> m_pTarget;
	float m_Speed = 0.f;
	int health = 0;
};