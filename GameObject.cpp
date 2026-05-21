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
	if (Type == ObjectType::WORD) {
		static float totalTime = 0.0f;
		totalTime += time;

		float amplitude = 0.5f;  // 위아래 움직이는 폭 (진폭)
		float frequency = 1.5f;  // 움직이는 속도 (주파수)

		float deltaY = amplitude * frequency * cosf(frequency * totalTime) * time;
		Position.y += deltaY;
		SetWorldMatrix();
	}
}