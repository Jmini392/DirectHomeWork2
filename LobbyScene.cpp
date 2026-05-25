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
	std::shared_ptr<CMesh> pMesh = std::make_shared<CCubeMesh>(Device, CommandList, XMFLOAT3(0.5f, 0.5f, 0.5f));
	std::shared_ptr<CMesh> StartMesh = std::make_shared<CObjMesh>(Device, CommandList, "Start.obj");
	std::shared_ptr<CMesh> ExitMesh = std::make_shared<CObjMesh>(Device, CommandList, "Exit.obj");

	// 쉐이더 생성
	std::shared_ptr<CShader> pShader = std::make_shared<CShader>();
	pShader->CreateShader(Device, GraphicsRootSignature);

	// 큐브 띄우기
	std::shared_ptr<CGameObject> pCube = std::make_shared<CGameObject>(pMesh, pShader,
		XMFLOAT3(2.f, 1.f, 2.f), XMFLOAT3(0.f, 0.f, 0.f), ObjectType::CUBE);
	AddGameObject(pCube);

	// 시작 띄우기
	std::shared_ptr<CGameObject> pStart = std::make_shared<CGameObject>(StartMesh, pShader,
		XMFLOAT3(0.f, 0.5f, 2.f), XMFLOAT3(0.f, 0.f, 0.f), ObjectType::START);
	pStart->SetColor(XMFLOAT4(0.f, 0.f, 0.f, 1.f));
	AddGameObject(pStart);

	// 종료 띄우기
	std::shared_ptr<CGameObject> pExit = std::make_shared<CGameObject>(ExitMesh, pShader,
		XMFLOAT3(0.f, -0.5f, 2.f), XMFLOAT3(0.f, 0.f, 0.f), ObjectType::EXIT);
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
	case WM_MOUSEMOVE:
	{
		int x = LOWORD(lParam);
		int y = HIWORD(lParam);

		// 카메라 행렬 가져오기
		XMMATRIX Proj = XMLoadFloat4x4(&camera->GetProjMatrix());
		XMMATRIX View = XMLoadFloat4x4(&camera->GetViewMatrix());

		XMVECTOR nearPoint = XMVectorSet((float)x, (float)y, 0.0f, 1.0f);
		XMVECTOR farPoint = XMVectorSet((float)x, (float)y, 1.0f, 1.0f);
		XMMATRIX Identity = XMMatrixIdentity();

		// 화면 좌표를 월드 공간으로 변환
		XMVECTOR nearWorld = XMVector3Unproject(nearPoint, 0, 0, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT, 0.0f, 1.0f, Proj, View, Identity);
		XMVECTOR farWorld = XMVector3Unproject(farPoint, 0, 0, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT, 0.0f, 1.0f, Proj, View, Identity);

		// 광선의 방향 계산
		XMVECTOR rayDir = XMVectorSubtract(farWorld, nearWorld);
		rayDir = XMVector3Normalize(rayDir);

		// 광선 충돌 전, 모든 객체의 색상을 원상 복구합니다.
		for (auto& pObj : m_GameObjects) {
			if (pObj->GetType() == ObjectType::START || pObj->GetType() == ObjectType::EXIT) {
				pObj->SetColor(XMFLOAT4(0.f, 0.f, 0.f, 1.f));
			}
		}

		// 여러 개가 클릭될 경우를 대비해 가장 가까운 거리의 객체를 찾습니다.
		float minDistance = FLT_MAX;
		std::shared_ptr<CGameObject> pickedObject = nullptr;

		// 충돌된 객체 중 가장 가까운 객체 찾기
		for (auto& pObj : m_GameObjects) {
			BoundingOrientedBox obb = pObj->GetWorldBoundingBox();
			float distance = 0.0f;

			if (obb.Intersects(nearWorld, rayDir, distance)) {
				if (distance < minDistance) {
					minDistance = distance;
					pickedObject = pObj;
				}
			}
		}

		if (pickedObject) {
			if (pickedObject->GetType() == ObjectType::START || pickedObject->GetType() == ObjectType::EXIT) {
				// 시작/종료 버튼에 마우스 오버 시 색상 변경
				pickedObject->SetColor(XMFLOAT4(1.f, 0.f, 0.f, 1.f));
			}
		}
		break;
	}
	case WM_LBUTTONDOWN:
	{
		int x = LOWORD(lParam);
		int y = HIWORD(lParam);

		// 카메라 행렬 가져오기
		XMMATRIX Proj = XMLoadFloat4x4(&camera->GetProjMatrix());
		XMMATRIX View = XMLoadFloat4x4(&camera->GetViewMatrix());

		XMVECTOR nearPoint = XMVectorSet((float)x, (float)y, 0.0f, 1.0f);
		XMVECTOR farPoint = XMVectorSet((float)x, (float)y, 1.0f, 1.0f);
		XMMATRIX Identity = XMMatrixIdentity();

		// 화면 좌표를 월드 공간으로 변환
		XMVECTOR nearWorld = XMVector3Unproject(nearPoint, 0, 0, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT, 0.0f, 1.0f, Proj, View, Identity);
		XMVECTOR farWorld = XMVector3Unproject(farPoint, 0, 0, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT, 0.0f, 1.0f, Proj, View, Identity);

		// 광선의 방향 계산
		XMVECTOR rayDir = XMVectorSubtract(farWorld, nearWorld);
		rayDir = XMVector3Normalize(rayDir);

		// 여러 개가 클릭될 경우를 대비해 가장 가까운 거리의 객체를 찾습니다.
		float minDistance = FLT_MAX;
		std::shared_ptr<CGameObject> pickedObject = nullptr;

		// 충돌된 객체 중 가장 가까운 객체 찾기
		for (auto& pObj : m_GameObjects) {
			BoundingOrientedBox obb = pObj->GetWorldBoundingBox();
			float distance = 0.0f;

			if (obb.Intersects(nearWorld, rayDir, distance)) {
				if (distance < minDistance) {
					minDistance = distance;
					pickedObject = pObj;
				}
			}
		}

		if (pickedObject) {
			if (pickedObject->GetType() == ObjectType::START) {
				// 시작 버튼 클릭 시 씬 전환
				isSceneChanged = true;
			}
			else if (pickedObject->GetType() == ObjectType::EXIT) {
				// 종료 버튼 클릭 시 게임 종료
				::PostQuitMessage(0);
			}
			else if (pickedObject->GetType() == ObjectType::CUBE) {
				// 큐브 클릭 시 맵 변경 및 색상 변경
				if (GetStageNum() == 1) {
					SetStageNum(2);
					pickedObject->SetColor(XMFLOAT4(1.f, 0.f, 0.f, 1.f)); // 빨강
				}
				else {
					SetStageNum(1);
					pickedObject->SetColor(XMFLOAT4(0.5f, 0.5f, 0.5f, 1.f)); // 회색
				}
			}
		}
		break;
	}
	default:
		break;
	}
}