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
		XMFLOAT3(0.f, -1.f, 2.f), XMFLOAT3(0.f, 0.f, 0.f), ObjectType::WORD);
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
	{
		int x = LOWORD(lParam);
		int y = HIWORD(lParam);

		// 카메라 행렬 가져오기
		XMMATRIX Proj = XMLoadFloat4x4(&camera->GetProjMatrix());
		XMMATRIX View = XMLoadFloat4x4(&camera->GetViewMatrix());

		// NDC 변환 방식을 없애고, DirectXMath에서 제공하는 XMVector3Unproject 사용을 권장합니다.
		// 이게 훨씬 안정적이고 실수가 적습니다.
		XMVECTOR nearPoint = XMVectorSet((float)x, (float)y, 0.0f, 1.0f);
		XMVECTOR farPoint = XMVectorSet((float)x, (float)y, 1.0f, 1.0f);

		// 카메라 Viewport 객체를 사용하지 못한다면 Frame Buffer Width와 Height로 넣어줍니다.
		// World 행렬은 항등 행렬(Identity)을 넣으면 World 공간상 좌표가 나옵니다.
		XMMATRIX Identity = XMMatrixIdentity();
		
		XMVECTOR nearWorld = XMVector3Unproject(nearPoint, 0, 0, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT, 0.0f, 1.0f, Proj, View, Identity);
		XMVECTOR farWorld  = XMVector3Unproject(farPoint, 0, 0, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT, 0.0f, 1.0f, Proj, View, Identity);

		// 광선의 방향 계산
		XMVECTOR rayDir = XMVectorSubtract(farWorld, nearWorld);
		rayDir = XMVector3Normalize(rayDir);

		// 충돌 검사
		for (auto& pObj : m_GameObjects) {
			BoundingOrientedBox obb = pObj->GetWorldBoundingBox();
			float distance = 0.0f;

			if (obb.Intersects(nearWorld, rayDir, distance)) {
				if (obb.Center.y > -0.5f) {
					isSceneChanged = true;
					SetStageNum(1); 
				} else {
					isSceneChanged = true;
					SetStageNum(2); 
				}
				break;
			}
		}
		break;
	}
	default:
		break;
	}
}