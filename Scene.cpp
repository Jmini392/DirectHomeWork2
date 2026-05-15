#include "Scene.h"
#include "TitleScene.h"
#include "LobbyScene.h"
#include "PlayScene.h"

void CSceneManager::Init() {
	std::unique_ptr<CTitleScene> pTitleScene = std::make_unique<CTitleScene>();
	ChangeScene(std::move(pTitleScene));
}

void CSceneManager::ChangeScene(std::unique_ptr<CScene> newScene) {
	CurrentScene = std::move(newScene);
	if (CurrentScene) CurrentScene->Enter();
}

void CSceneManager::Animation(float time) {
	if (CurrentScene) CurrentScene->AnimateObjects(time);

	SceneType nextType = CurrentScene->GetNextScene();
	if (nextType != SceneType::NONE) {
		switch (nextType) {
		case SceneType::TITLE:
			ChangeScene(std::make_unique<CTitleScene>());
			break;
		case SceneType::LOBBY:
			ChangeScene(std::make_unique<CLobbyScene>());
			break;
		case SceneType::PLAY:
			ChangeScene(std::make_unique<CPlayScene>());
			break;
		}
	}
}

void CSceneManager::Rendering(HDC hDC) {
	if (CurrentScene) CurrentScene->DrawObjects(hDC);
}

void CSceneManager::AddObject(std::shared_ptr<CGameObject> pObj) {
	if (CurrentScene) CurrentScene->AddGameObject(pObj);
}

void CSceneManager::Input() {
	if (CurrentScene) CurrentScene->Input();
}

void CSceneManager::MouseProcessing(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam) {
	if (CurrentScene) CurrentScene->MouseProcessing(hWnd, nMessageID, wParam, lParam);
}

void CSceneManager::KeyboardProcessing(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam) {
	if (CurrentScene) CurrentScene->KeyboardProcessing(hWnd, nMessageID, wParam, lParam);
}