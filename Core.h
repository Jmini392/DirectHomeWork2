#pragma once
#include "PCH.h"
#include "Scene.h"
#include "Time.h"

class Core {
	// 윈도우 핸들과 인스턴스 핸들
	HINSTANCE m_hInstance;
	HWND m_hWnd;

	// 화면 크기
	RECT m_ScreenRect;

	// 게임 씬 객체
	CSceneManager m_SceneManager;

	// 프레임 레이트 문자열 버퍼
	TCHAR m_pszFrameRate[50] = { 0 };

	// 게임 타이머 객체
	CTime m_GameTimer;

	int m_nWndClientWidth;
	int m_nWndClientHeight;

	//DXGI 팩토리 인터페이스에 대한 포인터이다.
	IDXGIFactory4* m_pdxgiFactory;

	//스왑 체인 인터페이스에 대한 포인터이다. 주로 디스플레이를 제어하기 위하여 필요하다. 
	IDXGISwapChain3* m_pdxgiSwapChain;

	//Direct3D 디바이스 인터페이스에 대한 포인터이다. 주로 리소스를 생성하기 위하여 필요하다.
	ID3D12Device* m_pd3dDevice;

	//MSAA 다중 샘플링을 활성화하고 다중 샘플링 레벨을 설정한다.
	bool m_bMsaa4xEnable = false;
	UINT m_nMsaa4xQualityLevels = 0;

	//스왑 체인의 후면 버퍼의 개수이다. 
	static const UINT m_nSwapChainBuffers = 2;

	//현재 스왑 체인의 후면 버퍼 인덱스이다.
	UINT m_nSwapChainBufferIndex;

	//렌더 타겟 버퍼, 서술자 힙 인터페이스 포인터, 렌더 타겟 서술자 원소의 크기이다.
	ID3D12Resource* m_ppd3dSwapChainBackBuffers[m_nSwapChainBuffers];
	ID3D12DescriptorHeap* m_pd3dRtvDescriptorHeap;
	UINT m_nRtvDescriptorIncrementSize;

	//깊이-스텐실 버퍼, 서술자 힙 인터페이스 포인터, 깊이-스텐실 서술자 원소의 크기이다.
	ID3D12Resource* m_pd3dDepthStencilBuffer;
	ID3D12DescriptorHeap* m_pd3dDsvDescriptorHeap;
	UINT m_nDsvDescriptorIncrementSize;

	//명령 큐, 명령 할당자, 명령 리스트 인터페이스 포인터이다.
	ID3D12CommandQueue* m_pd3dCommandQueue;
	ID3D12CommandAllocator* m_pd3dCommandAllocator;
	ID3D12GraphicsCommandList* m_pd3dCommandList;

	//그래픽스 파이프라인 상태 객체에 대한 인터페이스 포인터이다.
	ID3D12PipelineState* m_pd3dPipelineState;

	//펜스 인터페이스 포인터, 펜스의 값, 이벤트 핸들이다.
	ID3D12Fence* m_pd3dFence;
	UINT64 m_nFenceValue;
	HANDLE m_hFenceEvent;

	int m_nCurrentFrameRate;
public:
	// 생성과 소멸
	Core();
	~Core();
	
	// 초기화
	void OnCreate(HINSTANCE hInstance, HWND hWnd);
	// 종료
	void OnDestroy();
	// 프레임 진행 (반복)
	void FrameAdvance();

	// 키 입력
	void Input();
	// 키보드, 마우스 입력
	void KeyboardProcessing(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	void MouseProcessing(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);

	// 객체 생성
	void BuildObjects();
	// 객체 애니메이션
	void AnimateObjects();

	//스왑 체인, 디바이스, 서술자 힙, 명령 큐/할당자/리스트를 생성하는 함수이다.
	void CreateSwapChain();
	void CreateRtvAndDsvDescriptorHeaps();
	void CreateDirect3DDevice();
	void CreateCommandQueueAndList();
	
	//렌더 타겟 뷰와 깊이-스텐실 뷰를 생성하는 함수이다.
	void CreateRenderTargetViews();
	void CreateDepthStencilView();

	//CPU와 GPU를 동기화하는 함수이다.
	void WaitForGpuComplete();	

	void MoveToNextFrame();

	// 후면 버퍼마다 펜스값 관리
	UINT64 m_nFenceValues[m_nSwapChainBuffers];
};