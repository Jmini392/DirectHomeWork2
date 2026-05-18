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

void CGameObject::VindingMatrix(ID3D12GraphicsCommandList* pd3dCommandList) {
	XMFLOAT4X4 xmf4x4World;
	XMStoreFloat4x4(&xmf4x4World, XMMatrixTranspose(XMLoadFloat4x4(&WorldMatrix)));
	pd3dCommandList->SetGraphicsRoot32BitConstants(0, 16, &xmf4x4World, 0);
}

void CGameObject::Draw(ID3D12GraphicsCommandList* pd3dCommandList) {
	VindingMatrix(pd3dCommandList);
	if (shader) shader->Render(pd3dCommandList);
	if (mesh) mesh->IAVinding(pd3dCommandList);
}