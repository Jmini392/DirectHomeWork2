#pragma once
#include "PCH.h"
#include "Scene.h"
#include "Shader.h"
#include "Camera.h"
#include "GameObject.h"
#include "Player.h"

class CPlayScene : public CScene {
public:
	CPlayScene() {}
	virtual ~CPlayScene() {}

	void Enter(ID3D12Device* Device, ID3D12GraphicsCommandList* CommandList, ID3D12RootSignature* GraphicsRootSignature);
	void Exit();

	void Input();
	void KeyboardProcessing(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	void MouseProcessing(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);

	void BuildObjects(ID3D12Device* Device, ID3D12GraphicsCommandList* CommandList, ID3D12RootSignature* GraphicsRootSignature);
	void AnimateObjects(float time);
	void DrawObjects(ID3D12GraphicsCommandList* CommandList) override;

	void AddGameObject(std::shared_ptr<CGameObject> pObj) { m_GameObjects.push_back(pObj); }
	SceneType GetNextScene() override;
private:
	std::shared_ptr<CPlayer> m_Player;
	std::unique_ptr<CCamera> m_Camera;
	std::vector<std::shared_ptr<CGameObject>> m_GameObjects;
	
	POINT OldCursorPos;
};