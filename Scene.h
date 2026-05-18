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

    // 루트 시그니처 파라미터 추가
	virtual void Enter(ID3D12Device* Device, ID3D12GraphicsCommandList* CommandList, ID3D12RootSignature* GraphicsRootSignature) {}
	virtual void Exit() {}

	virtual void Input() {}
	virtual void KeyboardProcessing(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam) {}
	virtual void MouseProcessing(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam) {}

	virtual void BuildObjects() {}
	virtual void AnimateObjects(float time) {}
	virtual void DrawObjects(ID3D12GraphicsCommandList* CommandList) {}
	virtual void AddGameObject(std::shared_ptr<class CGameObject> pObj) {}

	virtual SceneType GetNextScene() { return SceneType::NONE; }
};

class CSceneManager {
public:
	CSceneManager() {}
	~CSceneManager() {}

	// 처음 시작할 씬 설정
	void Init(ID3D12Device* Device, ID3D12GraphicsCommandList* CommandList);
	// 새로운 씬으로 전환
	void ChangeScene(std::unique_ptr<CScene> newScene);
	// 현재 씬 업데이트
	void Animation(float time);
	// 현재 씬 렌더링
	void Rendering(ID3D12GraphicsCommandList* CommandList);
	// 현재 씬에 객체를 추가하는 함수
	void AddObject(std::shared_ptr<class CGameObject> pObj);
	// 현재 씬 입력처리
	void Input();
	// 현재 씬 키보드 입력처리
	void KeyboardProcessing(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	// 현재 씬 마우스 입력처리
	void MouseProcessing(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	//그래픽 루트 시그너쳐를 생성한다.
	ID3D12RootSignature* CreateGraphicsRootSignature(ID3D12Device* Device);
	ID3D12RootSignature* GetGraphicsRootSignature();
private:
	std::unique_ptr<CScene> CurrentScene;

	ID3D12RootSignature* m_GraphicsRootSignature;

	ID3D12Device* m_Device;
	ID3D12GraphicsCommandList* m_CommandList;
};