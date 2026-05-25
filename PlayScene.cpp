#include "PlayScene.h"

void CPlayScene::Enter(ID3D12Device* Device, ID3D12GraphicsCommandList* CommandList, ID3D12RootSignature* GraphicsRootSignature) {
    // BuildObjects로 전달
	BuildObjects(Device, CommandList, GraphicsRootSignature);
}

void CPlayScene::Exit() {

}

void CPlayScene::CreateMap(ID3D12Device* Device, ID3D12GraphicsCommandList* CommandList, std::shared_ptr<CShader> pShader) {

	// 맵 저장 파일 읽어오기
	std::ifstream file;
	if (GetStageNum() == 1) file.open("map1.txt");
	else file.open("map2.txt");
	if (!file.is_open()) {
		return;
	}
	std::string line;
	std::vector<std::vector<int>> mapData;
	while (std::getline(file, line)) {
		std::vector<int> row;
		for (char c : line) {
			if (c >= '0' && c <= '9') row.push_back(c - '0');
		}
		if (!row.empty()) mapData.push_back(row);
	}

	// 메쉬 생성
	std::shared_ptr<CMesh> PlaneMesh = std::make_shared<CCubeMesh>(Device, CommandList, XMFLOAT3(10.f, 0.f, 10.f));
	std::shared_ptr<CMesh> WallMesh = std::make_shared<CCubeMesh>(Device, CommandList, XMFLOAT3(10.f, 20.f, 10.f));
	std::shared_ptr<CMesh> SWallMesh = std::make_shared<CCubeMesh>(Device, CommandList, XMFLOAT3(10.f, 10.f, 10.f));
	std::shared_ptr<CMesh> StairMesh = std::make_shared<CCubeMesh>(Device, CommandList, XMFLOAT3(10.f, 10.f * ROOT2, 10.f * ROOT2));
	//std::shared_ptr<CMesh> StairMesh = std::make_shared<CStairMesh>(Device, CommandList);
	
	// 맵 생성
	for (int i = 0; i < mapData.size(); i++) {
		for (int j = 0; j < mapData[i].size(); j++) {
			XMFLOAT3 position = XMFLOAT3(j * 10.f, 3.f, i * -10.f);
			if (mapData[i][j] == 1) {
				std::shared_ptr<CGameObject> pWall = std::make_shared<CGameObject>(WallMesh, pShader,
					XMFLOAT3(position.x, position.y + 5.f, position.z), XMFLOAT3(0.f, 0.f, 0.f), ObjectType::WALL);
				if (GetStageNum() == 1) pWall->SetColor(XMFLOAT4(0.5f, 0.5f, 0.5f, 1.f));
				else pWall->SetColor(XMFLOAT4(0.5f, 0.f, 0.f, 1.f));
				AddGameObject(pWall);
			}
			else if (mapData[i][j] == 2) {
				std::shared_ptr<CGameObject> pSWall = std::make_shared<CGameObject>(SWallMesh, pShader,
					position, XMFLOAT3(0.f, 0.f, 0.f), ObjectType::WALL);
				if (GetStageNum() == 1) pSWall->SetColor(XMFLOAT4(0.25f, 0.25f, 0.25f, 1.f));
				else pSWall->SetColor(XMFLOAT4(1.f, 0.5f, 0.5f, 1.f));
				AddGameObject(pSWall);
			}
			else if (mapData[i][j] == 3) {
				std::shared_ptr<CGameObject> pStair = std::make_shared<CGameObject>(StairMesh, pShader,
					XMFLOAT3(position.x, position.y - 5.f, position.z + 5.f), XMFLOAT3(45.f, 0.f, 0.f), ObjectType::STAIR);
				if (GetStageNum() == 1) pStair->SetColor(XMFLOAT4(0.25f, 0.25f, 0.25f, 1.f));
				else pStair->SetColor(XMFLOAT4(1.f, 0.5f, 0.5f, 1.f));
				AddGameObject(pStair);
			}
			else if (mapData[i][j] == 4) {
				std::shared_ptr<CGameObject> pStair = std::make_shared<CGameObject>(StairMesh, pShader,
					XMFLOAT3(position.x + 5.f, position.y - 5.f, position.z), XMFLOAT3(45.f, 90.f, 0.f), ObjectType::STAIR);
				if (GetStageNum() == 1) pStair->SetColor(XMFLOAT4(0.25f, 0.25f, 0.25f, 1.f));
				else pStair->SetColor(XMFLOAT4(1.f, 0.5f, 0.5f, 1.f));
				AddGameObject(pStair);
			}
			else if (mapData[i][j] == 5) {
				std::shared_ptr<CGameObject> pStair = std::make_shared<CGameObject>(StairMesh, pShader,
					XMFLOAT3(position.x, position.y - 5.f, position.z - 5.f), XMFLOAT3(45.f, 0.f, 0.f), ObjectType::STAIR);
				if (GetStageNum() == 1) pStair->SetColor(XMFLOAT4(0.25f, 0.25f, 0.25f, 1.f));
				else pStair->SetColor(XMFLOAT4(1.f, 0.5f, 0.5f, 1.f));
				AddGameObject(pStair);
			}
			else if (mapData[i][j] == 6) {
				std::shared_ptr<CGameObject> pStair = std::make_shared<CGameObject>(StairMesh, pShader,
					XMFLOAT3(position.x - 5.f, position.y - 5.f, position.z), XMFLOAT3(45.f, 90.f, 0.f), ObjectType::STAIR);
				if (GetStageNum() == 1) pStair->SetColor(XMFLOAT4(0.25f, 0.25f, 0.25f, 1.f));
				else pStair->SetColor(XMFLOAT4(1.f, 0.5f, 0.5f, 1.f));
				AddGameObject(pStair);
			}
			else {
				std::shared_ptr<CGameObject> pPlane = std::make_shared<CGameObject>(PlaneMesh, pShader,
					XMFLOAT3(position.x, position.y - 5.f, position.z), XMFLOAT3(0.f, 0.f, 0.f), ObjectType::FLOOR);
				if (GetStageNum() == 1) pPlane->SetColor(XMFLOAT4(0.75f, 0.75f, 0.75f, 1.f));
				else pPlane->SetColor(XMFLOAT4(1.f, 0.75f, 0.75f, 1.f));
				AddGameObject(pPlane);
			}
		}
	}
}

void CPlayScene::BuildObjects(ID3D12Device* Device, ID3D12GraphicsCommandList* CommandList, ID3D12RootSignature* GraphicsRootSignature) {
	// 카메라 객체 생성
	m_Camera = std::make_unique<CCamera>();

	// 메쉬 생성
	std::shared_ptr<CMesh> PlayerMesh = std::make_shared<CCubeMesh>(Device, CommandList, XMFLOAT3(1.f, 2.f, 1.f));
	std::shared_ptr<CMesh> CrossMesh = std::make_shared<CCrosshairMesh>(Device, CommandList);
	std::shared_ptr<CMesh> StartMesh = std::make_shared<CObjMesh>(Device, CommandList, "Start.obj");
	std::shared_ptr<CMesh> ExitMesh = std::make_shared<CObjMesh>(Device, CommandList, "Exit.obj");
	std::shared_ptr<CMesh> KeyMesh = std::make_shared<CObjMesh>(Device, CommandList, "Key.obj");

	// 쉐이더 생성
	std::shared_ptr<CShader> pShader = std::make_shared<CShader>();
	pShader->CreateShader(Device, GraphicsRootSignature);

	// 플레이어 객체 생성
	m_Player = std::make_shared<CPlayer>(PlayerMesh, pShader,
		XMFLOAT3(60.f, 0.f, -60.f), XMFLOAT3(0.f, 0.f, 0.f), m_Camera.get());
	m_Player->SetColor(XMFLOAT4(0.f, 0.f, 1.f, 1.f));
	AddGameObject(m_Player);

	// 조준선 객체 생성
	std::shared_ptr<CGameObject> pCrossHair = std::make_shared<CGameObject>(CrossMesh, pShader,
		XMFLOAT3(0.f, 0.f, 2.f), XMFLOAT3(0.f, 0.f, 0.f), ObjectType::CROSS);
	pCrossHair->SetColor(XMFLOAT4(1.f, 0.f, 0.f, 1.f));
	m_Player->AddChild(pCrossHair);

	// 맵 생성
	CreateMap(Device, CommandList, pShader);

	// 시작 띄우기
	std::shared_ptr<CGameObject> pStart = std::make_shared<CGameObject>(StartMesh, pShader,
		XMFLOAT3(60.f, 0.f, -58.f), XMFLOAT3(0.f, 0.f, 0.f), ObjectType::START);
	pStart->SetColor(XMFLOAT4(1.f, 0.f, 0.f, 1.f));
	AddGameObject(pStart);

	// 종료 띄우기
	std::shared_ptr<CGameObject> pExit = std::make_shared<CGameObject>(ExitMesh, pShader,
		XMFLOAT3(110.f, 0.f, -110.f), XMFLOAT3(0.f, 180.f, 0.f), ObjectType::EXIT);
	pExit->SetColor(XMFLOAT4(0.f, 1.f, 0.f, 1.f));
	AddGameObject(pExit);

	// 열쇠 띄우기
	std::shared_ptr<CGameObject> pKey = std::make_shared<CGameObject>(KeyMesh, pShader,
		XMFLOAT3(60.f, 0.f, -48.f), XMFLOAT3(0.f, 0.f, 0.f), ObjectType::ITEM);
	pKey->SetColor(XMFLOAT4(1.f, 1.f, 0.f, 1.f));
	AddGameObject(pKey);
}

void CPlayScene::AnimateObjects(float time) {
	// 애니메이션
	for (auto pObj : m_GameObjects) {
		pObj->Animate(time);
	}

	// 충돌 검사
	CollisionCheck();	
}

void CPlayScene::CollisionCheck() {
	for (size_t i = 0; i < m_GameObjects.size(); ++i) {
		auto& obj1 = m_GameObjects[i];
		for (size_t j = i + 1; j < m_GameObjects.size(); ++j) {
			auto& obj2 = m_GameObjects[j];
			// 둘 중 하나라도 소멸 상태면 검사 제외
			if (obj1->isdead || obj2->isdead) continue;
			ObjectType type1 = obj1->GetType();
			ObjectType type2 = obj2->GetType();
			// 동족 간의 충돌 무시
			if (type1 == type2) continue;
			// 땅과 벽은 서로 충돌 무시
			if ((type1 == ObjectType::FLOOR && type2 == ObjectType::WALL) || (type1 == ObjectType::WALL && type2 == ObjectType::FLOOR)) continue;
			// 벽과 계단은 서로 충돌 무시
			if ((type1 == ObjectType::WALL && type2 == ObjectType::STAIR) || (type1 == ObjectType::STAIR && type2 == ObjectType::WALL)) continue;
			// 계단과 땅은 서로 충돌 무시
			if ((type1 == ObjectType::STAIR && type2 == ObjectType::FLOOR) || (type1 == ObjectType::FLOOR && type2 == ObjectType::STAIR)) continue;
			// 두 객체의 바운딩 박스 교차 검사
			if (obj1->GetWorldBoundingBox().Intersects(obj2->GetWorldBoundingBox())) {
				obj1->OnCollision(obj2);
				obj2->OnCollision(obj1);
			}
		}
	}
}

void CPlayScene::DrawObjects(ID3D12GraphicsCommandList* CommandList) {
	// -----------------------------------------------------------------------------
	// 오브젝트 그리기
	m_Camera->RSSetup(CommandList);
	m_Camera->VindingMatrix(CommandList);

	for (auto pObj : m_GameObjects) {
		pObj->Draw(CommandList);
	}
}

SceneType CPlayScene::GetNextScene() {
	return SceneType::NONE;
}

void CPlayScene::Input() {
	static UCHAR pKeyBuffer[256];
	if (::GetKeyboardState(pKeyBuffer)) {
		int dir = 0;
		if (pKeyBuffer['W'] & 0x80) dir = 1;
		if (pKeyBuffer['S'] & 0x80) dir = -1;
		if (pKeyBuffer['A'] & 0x80) dir = -2;
		if (pKeyBuffer['D'] & 0x80) dir = 2;
		m_Player->Move(dir);
	}

	// 캡처 상태일 때만 마우스 회전 동작
	if (MouseCaptured) {
		POINT CursorPos;
		GetCursorPos(&CursorPos); // 현재 커서 위치 가져오기
		float cxMouseDelta = (float)(CursorPos.x - OldCursorPos.x) / 3.0f; // 마우스 이동량을 3으로 나누어 회전 속도 조절
		float cyMouseDelta = (float)(CursorPos.y - OldCursorPos.y) / 3.0f;

		// 마우스의 이동이 있었을 때만 회전 및 커서 원위치
		if (cxMouseDelta != 0.0f || cyMouseDelta != 0.0f) {
			m_Player->Rotate(0.0f, cxMouseDelta, 0.0f); // 마우스의 X 이동량을 플레이어의 Y축 회전에 적용
			SetCursorPos(OldCursorPos.x, OldCursorPos.y); // 커서를 이전 위치로 되돌리기
		}
	}
}

void CPlayScene::KeyboardProcessing(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam) {
	switch (nMessageID) {
	case WM_KEYDOWN:
		switch (wParam) {
		case 'C':
			m_Player->TransPersonView();
			break;
		case VK_ESCAPE:
			// ESC 키를 누르면 캡처를 풀도록 변경 (선택사항)
			if (MouseCaptured) {
				MouseCaptured = false;
				ShowCursor(TRUE); // 커서 다시 표시
			}
			else ::PostQuitMessage(0);
			break;
		default:
			break;
		}
		break;
	default:
		break;
	}
}

void CPlayScene::MouseProcessing(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam) {
	switch (nMessageID) {
	case WM_LBUTTONDOWN:
		// 화면을 클릭했을 때 마우스 캡처 시작
		if (!MouseCaptured) {
			MouseCaptured = true;
			GetCursorPos(&OldCursorPos); // 현재 클릭한 위치를 중앙/기준점으로 설정
			ShowCursor(FALSE); // 마우스 캡처와 함께 시스템 커서를 숨김 (선택사항)
		}
		break;
	case WM_LBUTTONUP:
		break;
	case WM_MOUSEMOVE:
		break;
	default:
		break;
	}
}