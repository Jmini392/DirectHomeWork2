#include "LobbyScene.h"

void CLobbyScene::Enter() {
	isSceneChanged = false;
	isHelpWindowOpen = false;

	BuildObjects();
}

void CLobbyScene::Exit() {
	// 플레이 씬으로 전환
	isSceneChanged = true;
}

void CLobbyScene::BuildObjects() {}

void CLobbyScene::AnimateObjects(float time) {}

void CLobbyScene::DrawObjects(HDC hDC) {
	RECT rect;
	GetClipBox(hDC, &rect);

	HBRUSH hBrush = CreateSolidBrush(RGB(255, 255, 255));
	FillRect(hDC, &rect, hBrush);

	SetTextColor(hDC, RGB(0, 0, 0));
	SetBkMode(hDC, TRANSPARENT);

	if (isHelpWindowOpen) {
		// 게임 방법 창 화면
		HFONT hHelpFont = CreateFont(28, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE,
			DEFAULT_CHARSET, OUT_OUTLINE_PRECIS, CLIP_DEFAULT_PRECIS,
			CLEARTYPE_QUALITY, VARIABLE_PITCH, TEXT("Arial"));
		HFONT hOldFont = (HFONT)SelectObject(hDC, hHelpFont);

		TCHAR szHelpText[] = _T("[ How To Play ]\n\nW, A, S, D: Move\nMouse Move: Look Around\nLShift: Shoot\n\nPress 'F' to return.");
		DrawText(hDC, szHelpText, -1, &rect, DT_CENTER | DT_VCENTER);

		SelectObject(hDC, hOldFont);
		DeleteObject(hHelpFont);
	}
	else {
		// 메인 화면
		int centerY = (rect.bottom - rect.top) / 2;

		// 타이틀 폰트 (큰 글씨)
		HFONT hTitleFont = CreateFont(64, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE,
			DEFAULT_CHARSET, OUT_OUTLINE_PRECIS, CLIP_DEFAULT_PRECIS,
			CLEARTYPE_QUALITY, VARIABLE_PITCH, TEXT("Arial"));
		// 안내 폰트 (중간 글씨)
		HFONT hInfoFont = CreateFont(32, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE,
			DEFAULT_CHARSET, OUT_OUTLINE_PRECIS, CLIP_DEFAULT_PRECIS,
			CLEARTYPE_QUALITY, VARIABLE_PITCH, TEXT("Arial"));

		HFONT hOldFont = (HFONT)SelectObject(hDC, hTitleFont);
		RECT rectTitle = { 0, centerY - 120, rect.right, centerY };
		DrawText(hDC, _T("DIRECT SHOOTER"), -1, &rectTitle, DT_CENTER | DT_TOP | DT_SINGLELINE);

		SelectObject(hDC, hInfoFont);
		RECT rectStart = { 0, centerY + 30, rect.right, centerY + 80 };
		DrawText(hDC, _T("Press 'Space' to play"), -1, &rectStart, DT_CENTER | DT_TOP | DT_SINGLELINE);

		RECT rectHelp = { 0, centerY + 80, rect.right, centerY + 130 };
		DrawText(hDC, _T("Press 'F' to help"), -1, &rectHelp, DT_CENTER | DT_TOP | DT_SINGLELINE);

		SelectObject(hDC, hOldFont);
		DeleteObject(hTitleFont);
		DeleteObject(hInfoFont);
	}
	DeleteObject(hBrush);
}

SceneType CLobbyScene::GetNextScene() {
	if (isSceneChanged) return SceneType::PLAY;
	return SceneType::NONE;
}

void CLobbyScene::KeyboardProcessing(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam) {
	switch (nMessageID) {
	case WM_KEYDOWN:
		switch (wParam) {
		case VK_ESCAPE:
			::PostQuitMessage(0);
			break;
		case 'F':
			isHelpWindowOpen = !isHelpWindowOpen;
			break;
		case VK_SPACE:
			if (!isHelpWindowOpen) Exit();
			break;
		default:
			break;
		}
		break;
	default:
		break;
	}
}

void CLobbyScene::MouseProcessing(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam) {
	switch (nMessageID) {
	case WM_LBUTTONDOWN:
		break;
	default:
		break;
	}
}