#include "Scene.h"
#include "PlayScene.h"
#include "LobbyScene.h"

void CSceneManager::Init(ID3D12Device* Device, ID3D12GraphicsCommandList* CommandList) {
	m_Device = Device;
	m_CommandList = CommandList;

	m_GraphicsRootSignature = CreateGraphicsRootSignature(Device);

	//ChangeScene(std::make_unique<CPlayScene>());
	ChangeScene(std::make_unique<CLobbyScene>());
}

void CSceneManager::ChangeScene(std::unique_ptr<CScene> newScene) {
	CurrentScene = std::move(newScene);
	if (CurrentScene) CurrentScene->Enter(m_Device, m_CommandList, m_GraphicsRootSignature);
}

void CSceneManager::Animation(float time) {
	if (CurrentScene) CurrentScene->AnimateObjects(time);

	SceneType nextType = CurrentScene->GetNextScene();
	if (nextType != SceneType::NONE) {
		switch (nextType) {
		case SceneType::TITLE:
			//ChangeScene(std::make_unique<CTitleScene>());
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

void CSceneManager::Rendering(ID3D12GraphicsCommandList* CommandList) {
	if (m_GraphicsRootSignature) CommandList->SetGraphicsRootSignature(m_GraphicsRootSignature);
	if (CurrentScene) CurrentScene->DrawObjects(CommandList);
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

ID3D12RootSignature* CSceneManager::CreateGraphicsRootSignature(ID3D12Device* Device) {
	ID3D12RootSignature* pd3dGraphicsRootSignature = NULL;

	D3D12_ROOT_PARAMETER pd3dRootParameters[2];
	pd3dRootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS;
	pd3dRootParameters[0].Constants.Num32BitValues = 20;
	pd3dRootParameters[0].Constants.ShaderRegister = 0;
	pd3dRootParameters[0].Constants.RegisterSpace = 0;
	pd3dRootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;
	pd3dRootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS;
	pd3dRootParameters[1].Constants.Num32BitValues = 32;
	pd3dRootParameters[1].Constants.ShaderRegister = 1;
	pd3dRootParameters[1].Constants.RegisterSpace = 0;
	pd3dRootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;

	D3D12_ROOT_SIGNATURE_FLAGS d3dRootSignatureFlags =
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_PIXEL_SHADER_ROOT_ACCESS;
	D3D12_ROOT_SIGNATURE_DESC d3dRootSignatureDesc;
	::ZeroMemory(&d3dRootSignatureDesc, sizeof(D3D12_ROOT_SIGNATURE_DESC));
	d3dRootSignatureDesc.NumParameters = _countof(pd3dRootParameters);
	d3dRootSignatureDesc.pParameters = pd3dRootParameters;
	d3dRootSignatureDesc.NumStaticSamplers = 0;
	d3dRootSignatureDesc.pStaticSamplers = NULL;
	d3dRootSignatureDesc.Flags = d3dRootSignatureFlags;

	ID3DBlob* pd3dSignatureBlob = NULL;
	ID3DBlob* pd3dErrorBlob = NULL;
	::D3D12SerializeRootSignature(&d3dRootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &pd3dSignatureBlob, &pd3dErrorBlob);
	Device->CreateRootSignature(0, pd3dSignatureBlob->GetBufferPointer(), pd3dSignatureBlob->GetBufferSize(),
		__uuidof(ID3D12RootSignature), (void**)&pd3dGraphicsRootSignature);
	if (pd3dSignatureBlob) pd3dSignatureBlob->Release();
	if (pd3dErrorBlob) pd3dErrorBlob->Release();

	return(pd3dGraphicsRootSignature);
}

ID3D12RootSignature* CSceneManager::GetGraphicsRootSignature() {
	return(m_GraphicsRootSignature);
}