#include "Player.h"
#include "Scene.h"

CPlayer::CPlayer(std::shared_ptr<CMesh> pMesh, std::shared_ptr<CShader> pShader,
	XMFLOAT3 position, XMFLOAT3 rotation, CCamera* camera) {
	mesh = pMesh;
	shader = pShader;
	m_pCamera = camera;
	Position = position;
	Rotation = rotation;
	isFirstPersonView = true;
	Type = ObjectType::PLAYER;
	if (m_pCamera) {
		if (m_pCamera) m_pCamera->SetPosition(position.x, position.y, position.z);
		if (m_pCamera) m_pCamera->SetRotation(rotation.x, rotation.y, rotation.z);
		if (m_pCamera) m_pCamera->SetPersonView(isFirstPersonView);
	}
	SetWorldMatrix();
}

void CPlayer::Move(int dir) {
	if (dir == 0) return;
	
	m_PrevPosition = Position;
	
	XMFLOAT3 moveVec = direction;
	int moveSign = dir;
	if (dir == 2 || dir == -2) {
		moveVec = XMFLOAT3(-direction.z, direction.y, direction.x);
		moveSign = (dir == -2) ? 1 : -1;
	}
	
	CGameObject::Move(moveVec.x * moveSign * MoveSpeed, moveVec.y * moveSign * MoveSpeed, moveVec.z * moveSign * MoveSpeed);

	if (m_pCamera) {
		m_pCamera->SetTarget(Position.x, Position.y, Position.z);

		if (isFirstPersonView) m_pCamera->SetPosition(Position.x, Position.y, Position.z);
		else m_pCamera->SetPosition(Position.x - direction.x * cameraDistance,
			Position.y + cameraDistance / 2.f, Position.z - direction.z * cameraDistance);

		m_pCamera->SetViewMatrix();
	}

	SetWorldMatrix(); 

	for (auto& child : m_Children) {
		child->SetParentWorldMatrix(WorldMatrix);
		child->SetWorldMatrix();
	}
}

void CPlayer::Rotate(float x, float y, float z) {
	CGameObject::Rotate(x, y, z);

	// 카메라에도 플레이어와 완전히 동일한 회전값 세팅
	if (m_pCamera) {
		m_pCamera->SetRotation(Rotation.x, Rotation.y, Rotation.z);

		float yaw = XMConvertToRadians(Rotation.y);
		direction.x = sinf(yaw);
		direction.y = 0.0f;
		direction.z = cosf(yaw);

		m_pCamera->SetTarget(Position.x, Position.y, Position.z);

		if (isFirstPersonView) m_pCamera->SetPosition(Position.x, Position.y, Position.z);
		else m_pCamera->SetPosition(Position.x - direction.x * cameraDistance,
			Position.y + cameraDistance / 2.f, Position.z - direction.z * cameraDistance);

		m_pCamera->SetViewMatrix();
	}

	SetWorldMatrix();

	for (auto& child : m_Children) {
		child->SetParentWorldMatrix(WorldMatrix);
		child->SetWorldMatrix();
	}
}

void CPlayer::OnCollision(std::shared_ptr<CGameObject> pOther) {
	ObjectType otherType = pOther->GetType();
	if (otherType == ObjectType::ENEMY) {
		
	}
	else if (otherType == ObjectType::ITEM) {

	}
	else if (otherType == ObjectType::WALL) {
		
	}
}

void CPlayer::TransPersonView() {
	isFirstPersonView = !isFirstPersonView;
	if (m_pCamera) {
		m_pCamera->SetPersonView(isFirstPersonView);
		m_pCamera->SetRotation(Rotation.x, Rotation.y, Rotation.z);
		m_pCamera->SetTarget(Position.x, Position.y, Position.z);

		if (isFirstPersonView) m_pCamera->SetPosition(Position.x, Position.y, Position.z);
		else m_pCamera->SetPosition(Position.x - direction.x * cameraDistance,
			Position.y + cameraDistance / 2.f, Position.z - direction.z * cameraDistance);
		m_pCamera->SetViewMatrix();
	}
}

void CPlayer::AddChild(std::shared_ptr<CGameObject> pChild) {
	pChild->SetParentWorldMatrix(WorldMatrix);
	m_Children.push_back(pChild);
}

void CPlayer::Draw(ID3D12GraphicsCommandList* CommandList) {
	CGameObject::Draw(CommandList);
	for (auto& child : m_Children) {
		child->Draw(CommandList);
	}
}