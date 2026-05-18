#include "PlayScene.h"

void CPlayScene::Enter(ID3D12Device* Device, ID3D12GraphicsCommandList* CommandList, ID3D12RootSignature* GraphicsRootSignature) {
    // BuildObjects로 전달
	BuildObjects(Device, CommandList, GraphicsRootSignature);
}

void CPlayScene::Exit() {

}

void CPlayScene::BuildObjects(ID3D12Device* Device, ID3D12GraphicsCommandList* CommandList, ID3D12RootSignature* GraphicsRootSignature) {
	// 카메라 객체 생성
	m_Camera = std::make_unique<CCamera>();

	// 쉐이더 생성
	std::shared_ptr<CShader> pShader = std::make_shared<CShader>();
	pShader->CreateShader(Device, GraphicsRootSignature);

	// 플레이어 객체 생성
	m_Player = std::make_shared<CPlayer>(Device, CommandList);
	m_Player->SetCamera(m_Camera.get());
	m_Player->SetPosition(0.f, 0.f, 0.f);
	m_Player->SetRotation(0.f, 0.f, 0.f);
	AddGameObject(m_Player);

	// 상자 하나 띄우기
	std::shared_ptr<CGameObject> pBox = std::make_shared<CGameObject>();
	std::shared_ptr<CMesh> pBoxMesh = std::make_shared<CCubeMesh>(Device, CommandList, 4.f, 4.f, 4.f);
	pBox->SetMesh(pBoxMesh);
	pBox->SetShader(pShader);
	pBox->SetPosition(0.f, 0.f, 10.f);
	pBox->SetRotation(0.f, 0.f, 0.f);
	pBox->SetType(ObjectType::WALL);
	AddGameObject(pBox);
}

void CPlayScene::AnimateObjects(float time) {

}

void CPlayScene::DrawObjects(ID3D12GraphicsCommandList* CommandList) {
	// -----------------------------------------------------------------------------
	// 오브젝트 그리기
	m_Camera->RSSetup(CommandList);
	m_Camera->VindingMatrix(CommandList);

	for (auto pObj : m_GameObjects) {
		pObj->Draw(CommandList);
	}
}

SceneType CPlayScene::GetNextScene() {
	return SceneType::NONE;
}

void CPlayScene::Input() {
	//static UCHAR pKeyBuffer[256];
	//if (::GetKeyboardState(pKeyBuffer)) {
	//	int dir = 0;
	//	if (pKeyBuffer['W'] & 0x80) dir = 1;
	//	if (pKeyBuffer['S'] & 0x80) dir = -1;
	//	if (pKeyBuffer['A'] & 0x80) dir = -2;
	//	if (pKeyBuffer['D'] & 0x80) dir = 2;
	//	m_Player->Move(dir);
	//}

	//POINT CursorPos;
	//GetCursorPos(&CursorPos); // 현재 커서 위치 가져오기
	//float cxMouseDelta = (float)(CursorPos.x - OldCursorPos.x) / 3.0f; // 마우스 이동량을 3으로 나누어 회전 속도 조절
	//float cyMouseDelta = (float)(CursorPos.y - OldCursorPos.y) / 3.0f;

	//// 마우스의 이동이 있었을 때만 회전 및 커서 원위치
	//if (cxMouseDelta != 0.0f || cyMouseDelta != 0.0f) {
	//	m_Player->Rotate(0.0f, cxMouseDelta, 0.0f); // 마우스의 X 이동량을 플레이어의 Y축 회전에 적용
	//	SetCursorPos(OldCursorPos.x, OldCursorPos.y); // 커서를 이전 위치로 되돌리기
	//}
}

void CPlayScene::KeyboardProcessing(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam) {
	switch (nMessageID) {
	case WM_KEYDOWN:
		switch (wParam) {
		case VK_ESCAPE:
			exit(0);
			break;
		default:
			break;
		}
		break;
	default:
		break;
	}
}

void CPlayScene::MouseProcessing(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam) {
	switch (nMessageID) {
	case WM_LBUTTONDOWN:
		break;
	case WM_LBUTTONUP:
		break;
	case WM_MOUSEMOVE:
		break;
	default:
		break;
	}
}