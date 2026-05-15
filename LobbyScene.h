#pragma once
#include "PCH.h"
#include "Scene.h"

class CLobbyScene : public CScene {
public:
	CLobbyScene() {}
	virtual ~CLobbyScene() {}

	void Enter();
	void Exit();

	void KeyboardProcessing(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	void MouseProcessing(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);

	void BuildObjects();
	void AnimateObjects(float time);
	void DrawObjects(HDC hDC) override;

	SceneType GetNextScene() override;
private:
	float elapsedTime = 0.0f;
	bool isSceneChanged = false;
	bool isHelpWindowOpen = false;
	HBITMAP m_hTitleImage = NULL;
};