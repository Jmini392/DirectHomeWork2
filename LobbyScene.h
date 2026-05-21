#pragma once
#include "PCH.h"
#include "Scene.h"
#include "Shader.h"
#include "Camera.h"
#include "GameObject.h"

class CLobbyScene : public CScene {
public:
	CLobbyScene() {}
	virtual ~CLobbyScene() {}

	void Enter(ID3D12Device* Device, ID3D12GraphicsCommandList* CommandList, ID3D12RootSignature* GraphicsRootSignature);
	void Exit();

	void Input() {}
	void KeyboardProcessing(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	void MouseProcessing(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);

	void BuildObjects(ID3D12Device* Device, ID3D12GraphicsCommandList* CommandList, ID3D12RootSignature* GraphicsRootSignature);
	void AnimateObjects(float time);
	void DrawObjects(ID3D12GraphicsCommandList* CommandList) override;

	void AddGameObject(std::shared_ptr<CGameObject> pObj) { m_GameObjects.push_back(pObj); }
	SceneType GetNextScene() override;
private:
	std::unique_ptr<CCamera> camera = {};
	std::vector<std::shared_ptr<CGameObject>> m_GameObjects = {};

	POINT OldCursorPos = { 0, 0 };
	bool MouseCaptured = false;

	float elapsedTime = 0.0f;
	bool isSceneChanged = false;
	bool isHelpWindowOpen = false;
	HBITMAP m_hTitleImage = NULL;
};