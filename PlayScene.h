#pragma once
#include "PCH.h"
#include "Scene.h"
#include "PipeLine.h"
#include "Camera.h"
#include "GameObject.h"
#include "Player.h"
#include "Enemy.h"

class CPlayScene : public CScene {
public:
	CPlayScene() {}
	virtual ~CPlayScene() {}

	void Enter();
	void Exit();

	void Input();
	void KeyboardProcessing(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	void MouseProcessing(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);

	void BuildObjects();
	void AnimateObjects(float time);
	void DrawObjects(HDC hDC) override;

	void AddGameObject(std::shared_ptr<CGameObject> pObj) { m_GameObjects.push_back(pObj); }
	SceneType GetNextScene() override;
private:
	std::shared_ptr<CPlayer> m_Player;
	std::unique_ptr<CCamera> m_Camera;
	std::vector<std::shared_ptr<CGameObject>> m_GameObjects;
	
	POINT OldCursorPos;
	bool isSceneChanged = false;
	bool isMouseCaptured = true; // 마우스 캡처(회전 모드) 상태 변수 추가
	
	float enemySpawnTimer = 10.f; // 적 생성 타이머
	int totalSpawnedEnemies = 0; // 현재까지 스폰된 총 적의 수
	int enemyCount = 0; // 현재 필드에 존재하는 적의 수
	int maxEnemyCount = 15; // 최대 적 수 제한

	bool isGameOver = false; // 게임 오버 상태 변수
	bool isGameClear = false; // 게임 클리어 상태 변수
};