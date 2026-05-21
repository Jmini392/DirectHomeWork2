#include "PlayScene.h"

void CPlayScene::Enter(ID3D12Device* Device, ID3D12GraphicsCommandList* CommandList, ID3D12RootSignature* GraphicsRootSignature) {
    // BuildObjects로 전달
	BuildObjects(Device, CommandList, GraphicsRootSignature);
}

void CPlayScene::Exit() {

}

void CPlayScene::BuildObjects(ID3D12Device* Device, ID3D12GraphicsCommandList* CommandList, ID3D12RootSignature* GraphicsRootSignature) {
	// 카메라 객체 생성
	m_Camera = std::make_unique<CCamera>();

	// 메쉬 생성
	std::shared_ptr<CMesh> BoxMesh = std::make_shared<CCubeMesh>(Device, CommandList, XMFLOAT3(2.f, 2.f, 2.f));
	std::shared_ptr<CMesh> PlaneMesh = std::make_shared<CCubeMesh>(Device, CommandList, XMFLOAT3(10.f, 0.f, 10.f));
	std::shared_ptr<CMesh> WallMesh = std::make_shared<CCubeMesh>(Device, CommandList, XMFLOAT3(10.f, 20.f, 10.f));
	std::shared_ptr<CMesh> SWallMesh = std::make_shared<CCubeMesh>(Device, CommandList, XMFLOAT3(10.f, 10.f, 10.f));
	std::shared_ptr<CMesh> CrossMesh = std::make_shared<CCrosshairMesh>(Device, CommandList);
	std::shared_ptr<CMesh> BulletMesh = std::make_shared<CObjMesh>(Device, CommandList, "sphere.obj");
	std::shared_ptr<CMesh> StartMesh = std::make_shared<CObjMesh>(Device, CommandList, "Start.obj");
	std::shared_ptr<CMesh> ExitMesh = std::make_shared<CObjMesh>(Device, CommandList, "Exit.obj");

	// 쉐이더 생성
	std::shared_ptr<CShader> pShader = std::make_shared<CShader>();
	pShader->CreateShader(Device, GraphicsRootSignature);

	// 플레이어 객체 생성
	m_Player = std::make_shared<CPlayer>(BoxMesh, pShader,
		XMFLOAT3(0.f, 0.f, 0.f), XMFLOAT3(0.f, 0.f, 0.f), m_Camera.get());
	m_Player->SetColor(XMFLOAT4(0.f, 0.f, 1.f, 1.f));
	AddGameObject(m_Player);

	// 조준선 객체 생성
	std::shared_ptr<CGameObject> pCrossHair = std::make_shared<CGameObject>(CrossMesh, pShader,
		XMFLOAT3(0.f, 0.f, 2.f), XMFLOAT3(0.f, 0.f, 0.f), ObjectType::CROSS);
	pCrossHair->SetColor(XMFLOAT4(1.f, 0.f, 0.f, 1.f));
	m_Player->AddChild(pCrossHair);

	// 벽 띄우기
	std::shared_ptr<CGameObject> pWall = std::make_shared<CGameObject>(WallMesh, pShader,
		XMFLOAT3(0.f, 2.f, 10.f), XMFLOAT3(0.f, 0.f, 0.f), ObjectType::WALL);
	pWall->SetColor(XMFLOAT4(0.5f, 0.5f, 0.5f, 1.f));
	AddGameObject(pWall);

	// 낮은 벽 띄우기
	std::shared_ptr<CGameObject> pSWall = std::make_shared<CGameObject>(SWallMesh, pShader,
		XMFLOAT3(10.f, 2.f, 0.f), XMFLOAT3(0.f, 0.f, 0.f), ObjectType::WALL);
	pSWall->SetColor(XMFLOAT4(0.5f, 0.5f, 0.5f, 1.f));
	AddGameObject(pSWall);

	// 바닥 평면 띄우기
	for (int i = -5; i <= 5; i++) {
		for (int j = -5; j <= 5; j++) {
			std::shared_ptr<CGameObject> pPlane = std::make_shared<CGameObject>(PlaneMesh, pShader,
				XMFLOAT3(i * 10.f, -2.f, j * 10.f), XMFLOAT3(0.f, 0.f, 0.f), ObjectType::FLOOR);
			pPlane->SetColor(XMFLOAT4(0.75f, 0.75f, 0.75f, 1.f));
			AddGameObject(pPlane);
		}
	}

	// 시작 지점 띄우기
	std::shared_ptr<CGameObject> pStart = std::make_shared<CGameObject>(StartMesh, pShader,
		XMFLOAT3(0.f, 0.f, 2.f), XMFLOAT3(0.f, 0.f, 0.f), ObjectType::WORD);
	AddGameObject(pStart);

	// 종료 지점 띄우기
	std::shared_ptr<CGameObject> pExit = std::make_shared<CGameObject>(ExitMesh, pShader,
		XMFLOAT3(0.f, 0.f, 2.f), XMFLOAT3(0.f, 0.f, 0.f), ObjectType::WORD);
	pExit->SetColor(XMFLOAT4(0.f, 1.f, 0.f, 1.f));
	AddGameObject(pExit);
}

void CPlayScene::AnimateObjects(float time) {
	for (auto pObj : m_GameObjects) {
		pObj->Animate(time);
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
			else {
				exit(0);
			}
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