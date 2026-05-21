#include "LobbyScene.h"

void CLobbyScene::Enter(ID3D12Device* Device, ID3D12GraphicsCommandList* CommandList, ID3D12RootSignature* GraphicsRootSignature) {
	isSceneChanged = false;
	isHelpWindowOpen = false;

	BuildObjects(Device, CommandList, GraphicsRootSignature);
}

void CLobbyScene::Exit() {
	// 플레이 씬으로 전환
	isSceneChanged = true;
}

void CLobbyScene::BuildObjects(ID3D12Device* Device, ID3D12GraphicsCommandList* CommandList, ID3D12RootSignature* GraphicsRootSignature) {
	// 카메라 객체 생성
	camera = std::make_unique<CCamera>();

	// 메쉬 생성
	std::shared_ptr<CMesh> StartMesh = std::make_shared<CObjMesh>(Device, CommandList, "Start.obj");
	std::shared_ptr<CMesh> ExitMesh = std::make_shared<CObjMesh>(Device, CommandList, "Exit.obj");

	// 쉐이더 생성
	std::shared_ptr<CShader> pShader = std::make_shared<CShader>();
	pShader->CreateShader(Device, GraphicsRootSignature);

	// 시작 띄우기
	std::shared_ptr<CGameObject> pStart = std::make_shared<CWord>(StartMesh, pShader,
		XMFLOAT3(0.f, 0.f, 2.f), XMFLOAT3(0.f, 0.f, 0.f), ObjectType::WORD);
	pStart->SetColor(XMFLOAT4(0.f, 0.f, 0.f, 1.f));
	AddGameObject(pStart);

	// 종료 띄우기
	std::shared_ptr<CGameObject> pExit = std::make_shared<CWord>(ExitMesh, pShader,
		XMFLOAT3(0.f, 0.f, 2.f), XMFLOAT3(0.f, 0.f, 0.f), ObjectType::WORD);
	pExit->SetColor(XMFLOAT4(0.f, 0.f, 0.f, 1.f));
	AddGameObject(pExit);
}

void CLobbyScene::AnimateObjects(float time) {
	for (auto pObj : m_GameObjects) {
		pObj->Animate(time);
	}
}

void CLobbyScene::DrawObjects(ID3D12GraphicsCommandList* CommandList) {
	camera->RSSetup(CommandList);
	camera->VindingMatrix(CommandList);

	for (auto pObj : m_GameObjects) {
		pObj->Draw(CommandList);
	}
}

SceneType CLobbyScene::GetNextScene() {
	if (isSceneChanged) return SceneType::PLAY;
	return SceneType::NONE;
}

void CLobbyScene::KeyboardProcessing(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam) {
	switch (nMessageID) {
	case WM_KEYDOWN:
		switch (wParam) {
		case VK_ESCAPE:
			::PostQuitMessage(0);
			break;
		default:
			break;
		}
		break;
	default:
		break;
	}
}

void CLobbyScene::MouseProcessing(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam) {
	switch (nMessageID) {
	case WM_LBUTTONDOWN:
		break;
	default:
		break;
	}
}