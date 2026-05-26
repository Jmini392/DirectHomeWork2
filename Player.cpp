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
		m_pCamera->SetPosition(position.x, position.y, position.z);
		m_pCamera->SetRotation(rotation.x, rotation.y, rotation.z);
		m_pCamera->SetTarget(position.x, position.y, position.z);
		m_pCamera->SetPersonView(isFirstPersonView);		
		m_pCamera->SetViewMatrix();
	}
	SetWorldMatrix();
}

void CPlayer::UpdateLocation() {
	if (m_pCamera) {
		m_pCamera->SetTarget(Position.x, Position.y, Position.z);
		if (isFirstPersonView) m_pCamera->SetPosition(Position.x, Position.y, Position.z);
		else m_pCamera->SetPosition(Position.x - direction.x * cameraDistance,
			Position.y + cameraDistance / 2.f, Position.z - direction.z * cameraDistance);
		m_pCamera->SetViewMatrix();
	}

	SetWorldMatrix();

	for (auto& child : m_Children) {
		if (child->GetType() != ObjectType::BULLET) {
			child->SetParentWorldMatrix(WorldMatrix);
		}
		child->SetWorldMatrix();
	}
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

	UpdateLocation();
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
	}
	UpdateLocation();
}

void CPlayer::Animate(float time) {
	if (isfalling) {
		Position.y -= fallSpeed * time;
		fallSpeed += 9.8f * time; // 중력 가속도 적용
	}

	for (auto& child : m_Children) {
		child->Animate(time);
	}

	m_Children.erase(
		std::remove_if(m_Children.begin(), m_Children.end(),
			[](const std::shared_ptr<CGameObject>& child) {
				return child->isdead;
			}),
		m_Children.end()
	);

	UpdateLocation();

	isfalling = true; 
}

void CPlayer::OnCollision(std::shared_ptr<CGameObject> pOther) {
	ObjectType otherType = pOther->GetType();
	
	BoundingOrientedBox playerOBB = GetWorldBoundingBox();
	BoundingOrientedBox otherOBB = pOther->GetWorldBoundingBox();
	
	float playerFootY = Position.y - 1.f;
	float otherTopY = otherOBB.Center.y + otherOBB.Extents.y;

	if (otherType == ObjectType::ENEMY) {
	
	}
	else if (otherType == ObjectType::ITEM) {
	
	}
	else if (otherType == ObjectType::FLOOR) {
		XMVECTOR rayOrigin = XMLoadFloat3(&Position);
		rayOrigin = XMVectorSetY(rayOrigin, playerFootY + 1.0f);
		XMVECTOR rayDir = XMVectorSet(0.0f, -1.0f, 0.0f, 0.0f);
		float dist = 0.0f;

		if (otherOBB.Intersects(rayOrigin, rayDir, dist)) {
			float hitY = XMVectorGetY(rayOrigin) - dist;
			float heightDiff = hitY - playerFootY;

			// 바닥 위에 서 있거나 살짝 파고든 경우 (높이 차 허용 범위)
			if (heightDiff > -1.0f && heightDiff <= 1.0f) {
				Position.y = hitY + 1.f; // 실제 부딪힌 바닥면 기준 + 내 발 높이(1.f)
				isfalling = false;
				fallSpeed = 1.f;
			}
		}
	}
	else if (otherType == ObjectType::WALL) {
		// OBB 교차를 넘어, 레이로 발 밑 지지 확인
		XMVECTOR rayOrigin = XMLoadFloat3(&Position);
		rayOrigin = XMVectorSetY(rayOrigin, playerFootY + 1.0f);
		XMVECTOR rayDir = XMVectorSet(0.0f, -1.0f, 0.0f, 0.0f);
		float dist = 0.0f;

		if (otherOBB.Intersects(rayOrigin, rayDir, dist)) {
			float hitY = XMVectorGetY(rayOrigin) - dist;
			float heightDiff = hitY - playerFootY;
			
			// 벽 위를 확실히 밟음
			if (heightDiff > -1.0f && heightDiff <= 1.0f) { 
				Position.y = otherTopY + 1.f;
				isfalling = false;
				fallSpeed = 1.f;
			}
			// 벽 옆면에 부딪힘
			else if (heightDiff > 1.0f) {
				Position = m_PrevPosition;
			}
		}
	}
	else if (otherType == ObjectType::STAIR) {
		XMVECTOR rayOrigin = XMLoadFloat3(&Position);
		rayOrigin = XMVectorSetY(rayOrigin, playerFootY + 1.0f); 
		XMVECTOR rayDir = XMVectorSet(0.0f, -1.0f, 0.0f, 0.0f);

		float dist = 0.0f;

		if (otherOBB.Intersects(rayOrigin, rayDir, dist)) {
			float hitY = XMVectorGetY(rayOrigin) - dist;
			float heightDiff = hitY - playerFootY;
			const float MAX_STEP_HEIGHT = 1.5f;

			// 계단 꼭대기에 정확히 안착
			if (abs(otherTopY - hitY) < 0.2f && heightDiff > -1.0f && heightDiff <= MAX_STEP_HEIGHT) {
				Position.y = otherTopY + 1.f; 
				isfalling = false;
				fallSpeed = 1.f;
			}
			// 계단/경사면을 밟고 있음
			else if (heightDiff > -1.0f && heightDiff <= MAX_STEP_HEIGHT) {
				Position.y += heightDiff;
				isfalling = false;
				fallSpeed = 1.f;
			}
			// 계단 옆/뒷면에 부딪힌 상황 
			else if (heightDiff > MAX_STEP_HEIGHT) {
				Position = m_PrevPosition;
			}
		}
	}

	UpdateLocation();
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

void CPlayer::SetBullet(std::shared_ptr<CMesh> pMesh, std::shared_ptr<CShader> pShader) {
	m_BulletMesh = pMesh;
	m_BulletShader = pShader;
}

void CPlayer::Fire() {
	// 총알 생성 
	std::shared_ptr<CGameObject> bullet = std::make_shared<CBullet>(m_BulletMesh, m_BulletShader,
		XMFLOAT3(0.f, 0.f, 2.5f), Rotation, ObjectType::BULLET, XMFLOAT3(0.f, 0.f, 1.f));
	bullet->SetColor(XMFLOAT4(1.f, 1.f, 0.f, 1.f));
	
	AddChild(bullet);
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