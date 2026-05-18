#include "Player.h"
#include "Scene.h"

CPlayer::CPlayer(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList) {
	SetType(ObjectType::PLAYER);
	std::shared_ptr<CMesh> pEnemyMesh = std::make_shared<CCubeMesh>(pd3dDevice, pd3dCommandList, 4.f, 4.f, 4.f);
	SetMesh(pEnemyMesh);
}

void CPlayer::SetPosition(float x, float y, float z) {
	CGameObject::SetPosition(x, y, z);
	if (m_pCamera) m_pCamera->SetPosition(x, y, z);
}

void CPlayer::SetRotation(float x, float y, float z) {
	CGameObject::SetRotation(x, y, z);
	if (m_pCamera) m_pCamera->SetRotation(x, y, z);
	// 회전 후 방향 벡터 업데이트
	//XMFLOAT4X4 viewMatrix = m_pCamera->GetViewMatrix();
	//direction = XMFLOAT3(viewMatrix._13, viewMatrix._23, viewMatrix._33);
}

void CPlayer::Move(int dir) {
	if (dir == 0) return;
	
	// 이동 전 현재 위치 저장
	m_PrevPosition = GetPosition();
	
	XMFLOAT3 moveVec = direction;
	int moveSign = dir;
	if (dir == 2 || dir == -2) {
		moveVec = XMFLOAT3(-direction.z, direction.y, direction.x);
		moveSign = (dir == -2) ? 1 : -1;
	}
	
	CGameObject::Move(moveVec.x * moveSign * MoveSpeed, moveVec.y * moveSign * MoveSpeed, moveVec.z * moveSign * MoveSpeed);
	if (m_pCamera) m_pCamera->Move(moveVec.x * moveSign * MoveSpeed, moveVec.y * moveSign * MoveSpeed, moveVec.z * moveSign * MoveSpeed);
}

void CPlayer::Rotate(float x, float y, float z) {
	CGameObject::Rotate(x, y, z);
	if (m_pCamera) m_pCamera->Rotate(x, y, z);
	// 회전 후 방향 벡터 업데이트
	//XMFLOAT4X4 viewMatrix = m_pCamera->GetViewMatrix();
	//direction = XMFLOAT3(viewMatrix._13, viewMatrix._23, viewMatrix._33);
}

void CPlayer::OnCollision(std::shared_ptr<CGameObject> pOther) {
	ObjectType otherType = pOther->GetType();
	if (otherType == ObjectType::ENEMY) {
		isdead = true;
	}
	else if (otherType == ObjectType::ITEM) {

	}
	else if (otherType == ObjectType::WALL) {
		// 벽과 충돌 시 이동하기 이전 위치로 플레이어와 카메라 롤백
		CGameObject::SetPosition(m_PrevPosition.x, m_PrevPosition.y, m_PrevPosition.z);
		if (m_pCamera) {
			m_pCamera->SetPosition(m_PrevPosition.x, m_PrevPosition.y, m_PrevPosition.z);
		}
		SetWorldMatrix();
	}
}