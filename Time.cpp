#include "Time.h"

CTime::CTime() {
	FrameRate = 0;
	FpsCheckTime = 0.0f;
	m_fDeltaTime = 0.0f;
	PrevTime = std::chrono::high_resolution_clock::now();
}

void CTime::Tick(float TargetFrameRate) {
	// 목표 시간 대기 (Frame Capping)
	if (TargetFrameRate > 0.0f) {
		float targetDelay = 1.0f / TargetFrameRate;
		while (true) {
			CurrentTime = std::chrono::high_resolution_clock::now();
			std::chrono::duration<float> elapsed = CurrentTime - PrevTime;
			if (elapsed.count() >= targetDelay) {
				break;
			}
		}
	}
	else CurrentTime = std::chrono::high_resolution_clock::now();

	// 실제 경과 시간을 계산하고 변수에 저장
	std::chrono::duration<float> deltaTime = CurrentTime - PrevTime;
	m_fDeltaTime = deltaTime.count();
	PrevTime = CurrentTime;

	// 1초 단위의 FPS 산출
	static unsigned long frameCount = 0;
	frameCount++;
	FpsCheckTime += m_fDeltaTime;

	if (FpsCheckTime >= 1.0f) {
		FrameRate = frameCount;
		frameCount = 0;
		FpsCheckTime -= 1.0f;
	}
}

unsigned long CTime::PrintFrameRate(LPTSTR lpszString, int nCharacters) {
	if (lpszString) {
		_itow_s(FrameRate, lpszString, nCharacters, 10);
		wcscat_s(lpszString, nCharacters, _T(" FPS)"));
	}

	return(FrameRate);
}