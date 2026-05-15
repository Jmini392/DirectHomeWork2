#pragma once
#include "PCH.h"

class CTime {
public:
	CTime();
	virtual ~CTime() {}

	void Tick(float TargetFrameRate);

	float GetDeltatime() { return m_fDeltaTime; } 	
	unsigned long PrintFrameRate(LPTSTR lpszString = NULL, int nCharacters = 0);
private:
	// 마지막으로 틱이 기록된 시간
	std::chrono::steady_clock::time_point PrevTime;
	// 현재 틱이 기록된 시간
	std::chrono::steady_clock::time_point CurrentTime;
	// 1초 단위로 프레임 수를 체크하기 위한 시간
	float FpsCheckTime;
	// 1초당 프레임 수를 저장하는 변수
	unsigned long FrameRate = 0;
	// 실제 프레임당 경과 시간을 담을 변수 추가
	float m_fDeltaTime = 0.0f; 
};