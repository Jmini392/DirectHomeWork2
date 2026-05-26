#include "GameObject.h"

CGameObject::CGameObject(std::shared_ptr<CMesh> pMesh, std::shared_ptr<CShader> pShader,
	XMFLOAT3 position, XMFLOAT3 rotation, ObjectType type) {
	mesh = pMesh;
	shader = pShader;
	Position = position;
	Rotation = rotation;
	Type = type;
	SetWorldMatrix();
}

void CGameObject::SetWorldMatrix() {
	// 회전을 라디안으로 변환
	XMFLOAT3 radRotation;
	radRotation.x = XMConvertToRadians(Rotation.x);
	radRotation.y = XMConvertToRadians(Rotation.y);
	radRotation.z = XMConvertToRadians(Rotation.z);

	// 객체 회전을 쿼터니언으로 변환
	XMVECTOR quatRotation = XMQuaternionRotationRollPitchYawFromVector(XMLoadFloat3(&radRotation));
	// 회전 행렬 생성
	XMMATRIX mRotation = XMMatrixRotationQuaternion(quatRotation);
	// 이동 행렬 생성
	XMMATRIX mTranslation = XMMatrixTranslation(Position.x, Position.y, Position.z);
	// 월드 행렬 계산
	XMMATRIX mWorld = mRotation * mTranslation;

	// 부모 월드 행렬이 단위 행렬이 아닌 경우에만 곱셈 수행
	if (ParentWorldMatrix.m[0][0] != 1.f || ParentWorldMatrix.m[1][1] != 1.f || ParentWorldMatrix.m[2][2] != 1.f) {
		XMMATRIX mParentWorld = XMLoadFloat4x4(&ParentWorldMatrix);
		mWorld = mWorld * mParentWorld;
	}

	// 월드 행렬을 XMFLOAT4X4로 변환하여 저장
	XMStoreFloat4x4(&WorldMatrix, mWorld);
}

void CGameObject::VindingMatrix(ID3D12GraphicsCommandList* pd3dCommandList) {
	CB_GAMEOBJECT cbGameObject;
	XMStoreFloat4x4(&cbGameObject.WorldMatrix, XMMatrixTranspose(XMLoadFloat4x4(&WorldMatrix)));
	cbGameObject.Color = Color;

	pd3dCommandList->SetGraphicsRoot32BitConstants(0, 20, &cbGameObject, 0);
}

void CGameObject::Draw(ID3D12GraphicsCommandList* pd3dCommandList) {
	VindingMatrix(pd3dCommandList);
	if (shader) shader->Render(pd3dCommandList);
	if (mesh) mesh->IAVinding(pd3dCommandList);
}

void CGameObject::Animate(float time) {
	if (Type == ObjectType::CUBE) {
		Rotation.y += 45.f * time; // 초당 45도 회전
		SetWorldMatrix();
	}
}

CFloatingObject::CFloatingObject(std::shared_ptr<CMesh> pMesh, std::shared_ptr<CShader> pShader,
	XMFLOAT3 position, XMFLOAT3 rotation, ObjectType type) {
	mesh = pMesh;
	shader = pShader;
	Position = position;
	Rotation = rotation;
	Type = type;
	SetWorldMatrix();
}

void CFloatingObject::Animate(float time) {
	TotalTime += time;
	DeltaY = Amplitude * Frequency * cosf(Frequency * TotalTime) * time;
	Position.y += DeltaY;
	SetWorldMatrix();
}

CBullet::CBullet(std::shared_ptr<CMesh> pMesh, std::shared_ptr<CShader> pShader,
	XMFLOAT3 position, XMFLOAT3 rotation, ObjectType type, XMFLOAT3 direction) {
	mesh = pMesh;
	shader = pShader;
	Position = position;
	Rotation = rotation;
	Type = type;
	Direction = direction;
	Speed = 15.f; // 총알 이동 속도 설정
	LifeTime = 1.f; // 총알 수명 설정
}

void CBullet::Animate(float time) {
	if (isdead) return;

	// 총알 이동
	Position.x += Direction.x * Speed * time;
	Position.y += Direction.y * Speed * time;
	Position.z += Direction.z * Speed * time;

	// 수명 감소
	LifeTime -= time;
	if (LifeTime <= 0.f) {
		isdead = true; // 수명이 다하면 총알 제거
	}
}

void CBullet::OnCollision(std::shared_ptr<CGameObject> pOther) {
	if (pOther->GetType() == ObjectType::ENEMY) {
		isdead = true; // 총알 제거
	}
	else if (pOther->GetType() == ObjectType::WALL) {
		isdead = true; // 총알 제거
	}
	else if (pOther->GetType() == ObjectType::STAIR) {
		isdead = true; // 총알 제거
	}
}