#include "PlayScene.h"

void CPlayScene::Enter() {
	BuildObjects();

	// 씬 진입 시 초기 마우스 위치 셋업
	::GetCursorPos(&OldCursorPos);
	ShowCursor(FALSE);
	isMouseCaptured = true;
}

void CPlayScene::Exit() {
	ShowCursor(TRUE);
	isSceneChanged = true;
}

void CPlayScene::BuildObjects() {
	// 카메라 객체 생성
	m_Camera = std::make_unique<CCamera>();

	// 플레이어 객체 생성
	m_Player = std::make_shared<CPlayer>();
	m_Player->SetCamera(m_Camera.get());
	m_Player->SetPosition(0.f, 0.f, 0.f);
	m_Player->SetRotation(0.f, 0.f, 0.f);
	AddGameObject(m_Player);

	// 바닥 추가
	float tileSize = 20.0f;
	int gridSize = 20;
	int halfGrid = gridSize / 2;
	for (int i = -halfGrid; i < halfGrid; ++i) {
		for (int j = -halfGrid; j < halfGrid; ++j) {
			// 격자의 중심을 기준으로 타일 배치
			float posX = (i * tileSize) + (tileSize * 0.5f);
			float posZ = (j * tileSize) + (tileSize * 0.5f);

			std::shared_ptr<CFloor> pFloorObj = std::make_shared<CFloor>();
			pFloorObj->SetPosition(posX, -10.f, posZ);
			if ((i + j) % 2 == 0) pFloorObj->SetColor(RGB(255, 100, 100));
			else pFloorObj->SetColor(RGB(215, 60, 60));
			AddGameObject(pFloorObj);
		}
	}

	// 벽 추가
	for (int i = 0; i < 8; ++i) {
		float angle = i * 45.f * (XM_PI / 180.f);
		float x = 170 * cosf(angle);
		float z = 170 * sinf(angle);

		std::shared_ptr<CWall>pWall = std::make_shared<CWall>();
		pWall->SetPosition(x, 40.f, z);
		pWall->SetRotation(0.f, -angle, 0.f);
		if (i % 2 == 0) pWall->SetColor(RGB(150, 150, 150));
		else pWall->SetColor(RGB(100, 100, 100));
		AddGameObject(pWall);
	}
}

void CPlayScene::AnimateObjects(float time) {
	if (!isMouseCaptured || isGameOver || isGameClear) time = 0.f;

	// 적 스폰 (10초마다 소환)
	if (totalSpawnedEnemies < maxEnemyCount) {
		enemySpawnTimer += time;
		if (enemySpawnTimer >= 10.0f) {
			enemySpawnTimer -= 10.0f;
			for (int i = 0; i < 3; ++i) {
				if (totalSpawnedEnemies >= maxEnemyCount) break;

				std::shared_ptr<CEnemy> pNewEnemy = std::make_shared<CEnemy>(i);

				float angle = (FIELD_RANDOM * 45.f) * (XM_PI / 180.f);
				float spawnX = 150.f * cosf(angle);
				float spawnZ = 150.f * sinf(angle);

				pNewEnemy->SetPosition(spawnX, 0.f, spawnZ);
				pNewEnemy->SetTarget(m_Player);
				AddGameObject(pNewEnemy);
				enemyCount++;
				totalSpawnedEnemies++;
			}
		}
	}

	// 상태 갱신 및 죽은 객체 삭제
	for (auto it = m_GameObjects.begin(); it != m_GameObjects.end(); ) {
		(*it)->Animate(time);
		(*it)->SetWorldMatrix(); 

		if ((*it)->isdead) {
			if ((*it)->GetType() == ObjectType::PLAYER) {
				isGameOver = true;
			}
			else if ((*it)->GetType() == ObjectType::ENEMY) {
				// 죽은 위치에 아이템 드랍
				enemyCount--;
				std::shared_ptr<CItem> pItem = std::make_shared<CItem>();
				XMFLOAT3 enemyPos = (*it)->GetPosition();
				pItem->SetPosition(enemyPos.x, 0.f, enemyPos.z);
				AddGameObject(pItem);
			}
			it = m_GameObjects.erase(it);
		}
		else ++it;
	}

	// 전체 객체 순회
	for (size_t i = 0; i < m_GameObjects.size(); ++i) {
		auto& obj1 = m_GameObjects[i];
		for (size_t j = i + 1; j < m_GameObjects.size(); ++j) {
			auto& obj2 = m_GameObjects[j];
			// 둘 중 하나라도 소멸 상태면 검사 제외
			if (obj1->isdead || obj2->isdead) continue;
			ObjectType type1 = obj1->GetType();
			ObjectType type2 = obj2->GetType();
			// 바닥은 모든 충돌 체크에서 제외
			if (type1 == ObjectType::FLOOR || type2 == ObjectType::FLOOR) continue;
			// 동족 간의 충돌 무시
			if (type1 == type2) continue;
			// 두 객체의 바운딩 박스 교차 검사
			if (obj1->GetWorldBoundingBox().Intersects(obj2->GetWorldBoundingBox())) {
				obj1->OnCollision(obj2);
				obj2->OnCollision(obj1);
			}
		}
	}

	// 게임 클리어 조건 체크
	if (totalSpawnedEnemies >= maxEnemyCount && enemyCount == 0 && !isGameOver) {
		isGameClear = true;
	}
}

void CPlayScene::DrawObjects(HDC hDC) {
	// -----------------------------------------------------------------------------
	// 오브젝트 그리기
	CPipeLine mpipeline;

	HBITMAP hBitmap = (HBITMAP)::GetCurrentObject(hDC, OBJ_BITMAP);
	DIBSECTION ds;
	if (::GetObject(hBitmap, sizeof(DIBSECTION), &ds) == sizeof(DIBSECTION)) {
		mpipeline.SetPixelBuffer((DWORD*)ds.dsBm.bmBits);
	}

	mpipeline.InitZBuffer(FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT);
	mpipeline.ClearZBuffer();

	m_Camera->SetViewMatrix();
	m_Camera->SetProjMatrix();
	m_Camera->SetViewportMatrix();
	mpipeline.SetViewMatrix(m_Camera->GetViewMatrix());
	mpipeline.SetProjMatrix(m_Camera->GetProjMatrix());
	mpipeline.SetViewportMatrix(m_Camera->GetViewportMatrix());

	for (auto pObj : m_GameObjects) {
		pObj->SetWorldMatrix();
		mpipeline.SetWorldMatrix(pObj->GetWorldMatrix());
		if (pObj->GetMesh()) {
			COLORREF color = pObj->GetColor();
			CMesh* mesh = pObj->GetMesh().get();
			mpipeline.DrawObject(hDC, mesh, color);
		}
	}
	// ---------------------------------------------------------------------
	// UI 그리기
	SetTextColor(hDC, RGB(0, 0, 0));
	SetBkMode(hDC, TRANSPARENT); // 배경 투명

	// 폰트 생성
	HFONT hFont = CreateFont(24, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE,
		DEFAULT_CHARSET, OUT_OUTLINE_PRECIS, CLIP_DEFAULT_PRECIS,
		CLEARTYPE_QUALITY, VARIABLE_PITCH, TEXT("Arial"));
	HFONT hOldFont = (HFONT)SelectObject(hDC, hFont);

	// 남은 총알 수
	RECT rectPlayer = { 20, 20, 300, 50 };
	TCHAR szPlayerText[64];
	_stprintf_s(szPlayerText, _T("Bullet : %d"), m_Player->GetBulletCount());
	DrawText(hDC, szPlayerText, -1, &rectPlayer, DT_LEFT | DT_TOP | DT_SINGLELINE);

	// 남은 적의 수
	RECT rectEnemy = { 20, 50, 300, 80 };
	TCHAR szEnemyText[64];
	_stprintf_s(szEnemyText, _T("Enemy : %d"), enemyCount);
	DrawText(hDC, szEnemyText, -1, &rectEnemy, DT_LEFT | DT_TOP | DT_SINGLELINE);

	// 잡은 적 수
	RECT rectScore = { 20, 80, 300, 110 };
	TCHAR szScoreText[64];
	_stprintf_s(szScoreText, _T("%d / %d"), totalSpawnedEnemies, maxEnemyCount);
	DrawText(hDC, szScoreText, -1, &rectScore, DT_LEFT | DT_TOP | DT_SINGLELINE);

	// 게임 오버 혹은 게임 클리어
	if (isGameOver || isGameClear) {
		RECT rectMain;
		GetClipBox(hDC, &rectMain);
		int centerY = (rectMain.bottom - rectMain.top) / 2;

		HFONT hTitleFont = CreateFont(64, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE,
			DEFAULT_CHARSET, OUT_OUTLINE_PRECIS, CLIP_DEFAULT_PRECIS,
			CLEARTYPE_QUALITY, VARIABLE_PITCH, TEXT("Arial"));
		
		HFONT hInfoFont = CreateFont(32, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE,
			DEFAULT_CHARSET, OUT_OUTLINE_PRECIS, CLIP_DEFAULT_PRECIS,
			CLEARTYPE_QUALITY, VARIABLE_PITCH, TEXT("Arial"));

		HFONT hOldFont2 = (HFONT)SelectObject(hDC, hTitleFont);
		RECT rectTitle = { 0, centerY - 120, rectMain.right, centerY };
		if (isGameOver) DrawText(hDC, _T("Game Over!"), -1, &rectTitle, DT_CENTER | DT_TOP | DT_SINGLELINE);
		else if (isGameClear) DrawText(hDC, _T("Game Clear!"), -1, &rectTitle, DT_CENTER | DT_TOP | DT_SINGLELINE);
		
		SelectObject(hDC, hInfoFont);
		RECT rectInfo = { 0, centerY + 30, rectMain.right, centerY + 80 };
		DrawText(hDC, _T("Press 'R' to return"), -1, &rectInfo, DT_CENTER | DT_TOP | DT_SINGLELINE);

		SelectObject(hDC, hOldFont2);

		DeleteObject(hTitleFont);
		DeleteObject(hInfoFont);
	}
	SelectObject(hDC, hOldFont);
	DeleteObject(hFont);
}

SceneType CPlayScene::GetNextScene() {
	if (isSceneChanged) return SceneType::LOBBY;
	return SceneType::NONE;
}

void CPlayScene::Input() {
	if (isGameOver || isGameClear) return;

	if (isMouseCaptured) {
		static UCHAR pKeyBuffer[256];
		if (::GetKeyboardState(pKeyBuffer)) {
			int dir = 0;
			if (pKeyBuffer['W'] & 0x80) dir = 1;
			if (pKeyBuffer['S'] & 0x80) dir = -1;
			if (pKeyBuffer['A'] & 0x80) dir = -2;
			if (pKeyBuffer['D'] & 0x80) dir = 2;
			m_Player->Move(dir);
		}

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
		case 'R':
			if (isGameOver || isGameClear) Exit();
			break;
		case VK_SHIFT:
			if (isGameOver || isGameClear) return;
			if (isMouseCaptured) if (auto bullet = m_Player->Fire()) AddGameObject(bullet);
			break;
		case VK_ESCAPE:
			if (isGameOver || isGameClear) return;
			if (isMouseCaptured) {
				ShowCursor(TRUE);
				isMouseCaptured = false;
			}
			else {
				ShowCursor(FALSE);
				isMouseCaptured = true;
				::GetCursorPos(&OldCursorPos);
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
	if (isGameOver || isGameClear) return;
	switch (nMessageID) {
	case WM_LBUTTONDOWN:
		if (!isMouseCaptured) {
			::ShowCursor(FALSE);
			isMouseCaptured = true;
			::GetCursorPos(&OldCursorPos);
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