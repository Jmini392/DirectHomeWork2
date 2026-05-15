#include "Core.h"

Core::Core() {
	m_hInstance = nullptr;
	m_hWnd = nullptr;
	m_hDCFrameBuffer = nullptr;
	m_hBitmapFrameBuffer = nullptr;
}

Core::~Core() {}

void Core::OnCreate(HINSTANCE hInstance, HWND hWnd) {
	m_hInstance = hInstance;
	m_hWnd = hWnd;

	::GetClientRect(m_hWnd, &m_ScreenRect);
	HDC hDC = ::GetDC(m_hWnd);

	m_hDCFrameBuffer = ::CreateCompatibleDC(hDC);
	
	int width = m_ScreenRect.right - m_ScreenRect.left;
	int height = m_ScreenRect.bottom - m_ScreenRect.top;

	BITMAPINFO bmi = {0};
	bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bmi.bmiHeader.biWidth = width;
	bmi.bmiHeader.biHeight = -height;
	bmi.bmiHeader.biPlanes = 1;
	bmi.bmiHeader.biBitCount = 32;
	bmi.bmiHeader.biCompression = BI_RGB;

	// m_pPixelBuffer에 픽셀 배열의 시작 주소가 담기게
	m_hBitmapFrameBuffer = ::CreateDIBSection(hDC, &bmi, DIB_RGB_COLORS, (void**)&m_pPixelBuffer, NULL, 0);

	::SelectObject(m_hDCFrameBuffer, m_hBitmapFrameBuffer);
	::ReleaseDC(m_hWnd, hDC);
	::SetBkMode(m_hDCFrameBuffer, TRANSPARENT);

	BuildObjects();

	_tcscpy_s(m_pszFrameRate, _T("FabProject ("));
}

void Core::OnDestroy() {
	ReleaseObjects();
	m_hInstance = nullptr;
	m_hWnd = nullptr;
}

void Core::FrameAdvance() {
	m_GameTimer.Tick(60.f);

	Input();
	
	AnimateObjects();

	ClearScreen();

	m_SceneManager.Rendering(m_hDCFrameBuffer);

	DrawScreen();

	m_GameTimer.PrintFrameRate(m_pszFrameRate + 12, 37);
	::SetWindowText(m_hWnd, m_pszFrameRate);
}

void Core::Input() {
	m_SceneManager.Input();
}

void Core::MouseProcessing(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam) {
	m_SceneManager.MouseProcessing(hWnd, nMessageID, wParam, lParam);
}

void Core::KeyboardProcessing(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam) {
	m_SceneManager.KeyboardProcessing(hWnd, nMessageID, wParam, lParam);
}

void Core::BuildObjects() {
	m_SceneManager.Init();
}

void Core::ReleaseObjects() {}

void Core::AnimateObjects() {
	float deltatime = m_GameTimer.GetDeltatime();
	m_SceneManager.Animation(deltatime);
}

void Core::DrawScreen() {
	HDC hDC = ::GetDC(m_hWnd);
	
	::BitBlt(hDC, m_ScreenRect.left, m_ScreenRect.top, m_ScreenRect.right - m_ScreenRect.left,
		m_ScreenRect.bottom - m_ScreenRect.top, m_hDCFrameBuffer, m_ScreenRect.left, m_ScreenRect.top, SRCCOPY);
	
	::ReleaseDC(m_hWnd, hDC);
}

void Core::ClearScreen() {
	COLORREF color = RGB(255, 255, 255);
	HPEN hPen = ::CreatePen(PS_SOLID, 0, color);
	HPEN hOldPen = (HPEN)::SelectObject(m_hDCFrameBuffer, hPen);
	HBRUSH hBrush = ::CreateSolidBrush(color);
	HBRUSH hOldBrush = (HBRUSH)::SelectObject(m_hDCFrameBuffer, hBrush);

	::Rectangle(m_hDCFrameBuffer, m_ScreenRect.left, m_ScreenRect.top, m_ScreenRect.right, m_ScreenRect.bottom);
	
	::SelectObject(m_hDCFrameBuffer, hOldPen);
	::SelectObject(m_hDCFrameBuffer, hOldBrush);
	::DeleteObject(hPen);
	::DeleteObject(hBrush);
}