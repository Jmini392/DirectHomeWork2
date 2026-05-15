#include "GameObject.h"

void CGameObject::SetWorldMatrix() {
	// 객체 회전을 쿼터니언으로 변환
	XMVECTOR quatRotation = XMQuaternionRotationRollPitchYawFromVector(XMLoadFloat3(&Rotation));
	// 회전 행렬 생성
	XMMATRIX mRotation = XMMatrixRotationQuaternion(quatRotation);
	// 이동 행렬 생성
	XMMATRIX mTranslation = XMMatrixTranslation(Position.x, Position.y, Position.z);
	// 월드 행렬 계산
	XMMATRIX mWorld = mRotation * mTranslation;
	// 월드 행렬을 XMFLOAT4X4로 변환하여 저장
	XMStoreFloat4x4(&WorldMatrix, mWorld);
}

CWall::CWall() {
	SetType(ObjectType::WALL);
	std::shared_ptr<CMesh> pWallMesh = std::make_shared<CCubeMesh>(2.f, 100.f, 140.f);
	SetMesh(pWallMesh);
}

CFloor::CFloor() {
	SetType(ObjectType::FLOOR);
	std::shared_ptr<CMesh> pFloorMesh = std::make_shared<CPlaneMesh>(20.f, 20.f);
	SetMesh(pFloorMesh);
}

CItem::CItem() {
	SetType(ObjectType::ITEM);
	randomValue = RANDOM;
	std::shared_ptr<CMesh> pItemMesh = std::make_shared<CCubeMesh>(1.5f, 1.5f, 1.5f);
	SetMesh(pItemMesh);
	if (randomValue == 0) SetColor(RGB(255, 0, 255));
	else SetColor(RGB(0, 255, 255));
}

void CItem::Animate(float time) {
	// 회전 애니메이션
	float y;
	y = GetRotation().y;
	y += 1.f * time;
	if (y > XM_2PI) y -= XM_2PI;
	SetRotation(GetRotation().x, y, GetRotation().z);
}

void CItem::OnCollision(std::shared_ptr<CGameObject> pOther) {
	if (pOther->GetType() == ObjectType::PLAYER) {
		isdead = true;
	}
}

void CBullet::Animate(float time) {
	XMFLOAT3 pos = GetPosition();

	// 방향 벡터를 위치에 더해서 이동
	pos = Vector3::Add(pos, Vector3::ScalarProduct(Direction, Speed * time)); 
	
	CGameObject::SetPosition(pos.x, pos.y, pos.z);

	// 총알이 생기고 일정거리 멀어지면 소멸 처리
	XMFLOAT3 offset = Vector3::Subtract(pos, StartPosition);
	if (Vector3::Length(offset) > 20.f) isdead = true;
}

void CBullet::OnCollision(std::shared_ptr<CGameObject> pOther) {
	ObjectType otherType = pOther->GetType();
	if (otherType == ObjectType::ENEMY || otherType == ObjectType::WALL) {
		isdead = true;
	}
}