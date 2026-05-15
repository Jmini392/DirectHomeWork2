#pragma once
#include "PCH.h"

enum class SceneType {
	NONE,
	TITLE,
	LOBBY,
	PLAY,
};

class CScene {
public:
	CScene() {}
	virtual ~CScene() {}

	virtual void Enter() {}
	virtual void Exit() {}

	virtual void Input() {}
	virtual void KeyboardProcessing(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam) {}
	virtual void MouseProcessing(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam) {}

	virtual void BuildObjects() {}
	virtual void AnimateObjects(float time) {}
	virtual void DrawObjects(HDC hDC) {}
	virtual void AddGameObject(std::shared_ptr<class CGameObject> pObj) {}

	virtual SceneType GetNextScene() { return SceneType::NONE; }
};

class CSceneManager {
public:
	CSceneManager() {}
	~CSceneManager() {}

	// 처음 시작할 씬 설정
	void Init();
	// 새로운 씬으로 전환
	void ChangeScene(std::unique_ptr<CScene> newScene);
	// 현재 씬 업데이트
	void Animation(float time);
	// 현재 씬 렌더링
	void Rendering(HDC hDC);
	// 현재 씬에 객체를 추가하는 함수
	void AddObject(std::shared_ptr<class CGameObject> pObj);
	// 씬 입력처리
	void Input();
	// 씬 키보드 입력처리
	void KeyboardProcessing(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	// 씬 마우스 입력처리
	void MouseProcessing(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
private:
	std::unique_ptr<CScene> CurrentScene;
};