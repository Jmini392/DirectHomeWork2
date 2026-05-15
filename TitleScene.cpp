#include "TitleScene.h"

CTitleScene::~CTitleScene() {
	if (m_hTitleImage) {
		DeleteObject(m_hTitleImage);
		m_hTitleImage = NULL;
	}
}

void CTitleScene::Enter() {
	elapsedTime = 0.0f;
	isSceneChanged = false;
	
	m_hTitleImage = (HBITMAP)LoadImage(NULL, _T("title.bmp"), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_CREATEDIBSECTION);

	BuildObjects();
}

void CTitleScene::Exit() {
	isSceneChanged = true;
}

void CTitleScene::BuildObjects() {}

void CTitleScene::AnimateObjects(float time) {
	elapsedTime += time;

	if (elapsedTime >= 3.0f) Exit();
}

void CTitleScene::DrawObjects(HDC hDC) {
	RECT rect;
	GetClipBox(hDC, &rect);

	if (m_hTitleImage != NULL) {
		HDC hMemDC = CreateCompatibleDC(hDC);

		HBITMAP hOldBitmap = (HBITMAP)SelectObject(hMemDC, m_hTitleImage);

		BITMAP bmp;
		GetObject(m_hTitleImage, sizeof(BITMAP), &bmp);

		StretchBlt(hDC, rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top,
			hMemDC, 0, 0, bmp.bmWidth, bmp.bmHeight, SRCCOPY);

		SelectObject(hMemDC, hOldBitmap);
		DeleteDC(hMemDC);
	}
}

SceneType CTitleScene::GetNextScene() {
	if (isSceneChanged) return SceneType::LOBBY;
	return SceneType::NONE;
}

void CTitleScene::KeyboardProcessing(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam) {
	switch (nMessageID) {
	case WM_KEYDOWN:
		switch (wParam) {
		case VK_ESCAPE:
			::PostQuitMessage(0);
			break;
		default:
			break;
		}
		break;
	default:
		break;
	}
}

void CTitleScene::MouseProcessing(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam) {}