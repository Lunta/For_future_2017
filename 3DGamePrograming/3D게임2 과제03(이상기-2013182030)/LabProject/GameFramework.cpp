//-----------------------------------------------------------------------------
// File: CGameFramework.cpp
//-----------------------------------------------------------------------------

#include "stdafx.h"
#include "GameFramework.h"

CGameFramework::CGameFramework():
	m_hInstance(NULL),
	m_hWnd(NULL),
	m_nWndClientWidth(FRAME_BUFFER_WIDTH),
	m_nWndClientHeight(FRAME_BUFFER_HEIGHT),
	m_pScene(NULL),
	m_pCamera(NULL),
	m_pPlayer(NULL),
	m_ptOldCursorPos({ 0, 0 }),

	m_pd3dDeviceIndRes(NULL),
	m_nSwapChainBufferIndex(0),
	m_nRtvDescriptorIncrementSize(0),
	m_nDsvDescriptorIncrementSize(0),
	m_hFenceEvent(NULL)
{
	_tcscpy_s(m_pszFrameRate, _T("LapProject ("));
	
	for (int i = 0; i < m_nSwapChainBuffers; i++) 
		m_nFenceValues[i] = 0;
}
CGameFramework::~CGameFramework()
{
}

bool CGameFramework::OnCreate(HINSTANCE hInstance, HWND hMainWnd)
{
    srand((unsigned int)time(NULL));

	m_hInstance = hInstance;
	m_hWnd = hMainWnd;

	RECT rcClient;
	::GetClientRect(m_hWnd, &rcClient);
	m_nWndClientWidth = rcClient.right - rcClient.left;
	m_nWndClientHeight = rcClient.bottom - rcClient.top;

	m_pd3dDeviceIndRes = new CD3DDeviceIndRes();
	m_pd3dDeviceIndRes->Initialize();

	CreateCommandQueueAndList();
	CreateSwapChain();
	CreateRtvAndDsvDescriptorHeaps();

	m_pd3dDeviceIndRes->CreateFence(
		  0
		, D3D12_FENCE_FLAG_NONE
		, &m_pd3dFence);
	m_hFenceEvent = ::CreateEvent(NULL, FALSE, FALSE, NULL);

	BuildObjects();

	return(true);
}
void CGameFramework::OnDestroy()
{
	ReleaseObjects();

	if (m_hWnd) DestroyWindow(m_hWnd);

	m_pdxgiSwapChain->SetFullscreenState(FALSE, NULL);
	::CloseHandle(m_hFenceEvent);
}
void CGameFramework::OnResizeBackBuffers()
{
	WaitForGpuComplete();
	m_pd3dCommandList->Reset(m_pd3dCommandAllocator.Get(), NULL);
	{
		for (int i = 0; i < m_nSwapChainBuffers; i++)
		{
			if (m_pd3dRenderTargetBuffers[i])
				m_pd3dRenderTargetBuffers[i].Reset();
		}
		if (m_pd3dDepthStencilBuffer) 
			m_pd3dDepthStencilBuffer.Reset();

#ifdef _WITH_ONLY_RESIZE_BACKBUFFERS
		DXGI_SWAP_CHAIN_DESC dxgiSwapChainDesc;
		m_pdxgiSwapChain->GetDesc(&dxgiSwapChainDesc);
		m_pdxgiSwapChain->ResizeBuffers(0, 0, 0, DXGI_FORMAT_UNKNOWN, 0);
		m_nSwapChainBufferIndex = 0;
#else
		DXGI_SWAP_CHAIN_DESC dxgiSwapChainDesc;
		m_pdxgiSwapChain->GetDesc(&dxgiSwapChainDesc);
		m_pdxgiSwapChain->ResizeBuffers(
			  m_nSwapChainBuffers
			, m_nWndClientWidth
			, m_nWndClientHeight
			, dxgiSwapChainDesc.BufferDesc.Format
			, dxgiSwapChainDesc.Flags);
		m_nSwapChainBufferIndex = 0;
#endif
		CreateRenderTargetView();
		CreateDepthStencilView();
	}
	m_pd3dCommandList->Close();

	ID3D12CommandList *ppd3dCommandLists[] = { m_pd3dCommandList.Get() };
	m_pd3dCommandQueue->ExecuteCommandLists(1, ppd3dCommandLists);

	WaitForGpuComplete();
}

void CGameFramework::OnProcessingMouseMessage(
	HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	if (m_pScene)
		m_pScene->OnProcessingMouseMessage(hWnd, nMessageID, wParam, lParam);
	else
	{
		switch (nMessageID)
		{
		case WM_LBUTTONDOWN:
		case WM_RBUTTONDOWN:
			//마우스가 눌려지면 마우스 픽킹을 하여 선택한 게임 객체를 찾는다.
			//m_pSelectedObject = m_pScene->PickObjectPointedByCursor(LOWORD(lParam),
			//	HIWORD(lParam), m_pCamera);
			//마우스 캡쳐를 하고 현재 마우스 위치를 가져온다.
			::SetCapture(hWnd);
			::GetCursorPos(&m_ptOldCursorPos);
			break;
		case WM_LBUTTONUP:
		case WM_RBUTTONUP:
			//마우스 캡쳐를 해제한다.
			::ReleaseCapture();
			break;
		case WM_MOUSEMOVE:
			break;
		default:
			break;
		}
	}
}
void CGameFramework::OnProcessingKeyboardMessage(
	HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{

	switch (nMessageID)
	{
	case WM_KEYUP:
		switch (wParam)
		{
			/* ‘F1’ 키를 누르면 1인칭 카메라, 
			// ‘F2’ 키를 누르면 스페이스-쉽 카메라로 변경한다, 
			// ‘F3’ 키를 누르면 3인칭 카메라 로 변경한다. */
		case VK_F1:
		case VK_F2:
		case VK_F3:
			if (m_pPlayer) 
				m_pPlayer->ChangeCamera((wParam - VK_F1 + 1), m_GameTimer.GetTimeElapsed());
			m_pCamera = m_pPlayer->GetCamera();
			break;
		case VK_ESCAPE:
			::PostQuitMessage(0);
			break;
		case VK_RETURN:
			break;
		case VK_F9:
			ToggleFullScreen();
			break;
		default:
			break;
		}
		break;
	default:
		break;
	}
}
LRESULT CALLBACK CGameFramework::OnProcessingWindowMessage(
	HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	switch (nMessageID)
	{
	case WM_ACTIVATE:
	{
		if (LOWORD(wParam) == WA_INACTIVE)
			m_GameTimer.Stop();
		else
			m_GameTimer.Start();
		break;
	}
	case WM_SIZE:
	{
		m_nWndClientWidth = LOWORD(lParam);
		m_nWndClientHeight = HIWORD(lParam);
		OnResizeBackBuffers();
		break;
	}
	case WM_LBUTTONDOWN:
	case WM_RBUTTONDOWN:
	case WM_LBUTTONUP:
	case WM_RBUTTONUP:
	case WM_MOUSEMOVE:
		OnProcessingMouseMessage(hWnd, nMessageID, wParam, lParam);
		break;
	case WM_KEYDOWN:
	case WM_KEYUP:
		OnProcessingKeyboardMessage(hWnd, nMessageID, wParam, lParam);
		break;
	}
	return(0);
}

//#define _WITH_PLAYER_TOP
void CGameFramework::FrameAdvance()
{
	m_GameTimer.Tick(0.0f);
	ProcessInput();
	AnimateObjects();

	m_pd3dCommandAllocator->Reset();
	m_pd3dCommandList->Reset(m_pd3dCommandAllocator.Get(), NULL);
	{
		D3D12_RESOURCE_BARRIER d3dResourceBarrier;
		::ZeroMemory(&d3dResourceBarrier, sizeof(D3D12_RESOURCE_BARRIER));
		d3dResourceBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		d3dResourceBarrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
		d3dResourceBarrier.Transition.pResource =
			m_pd3dRenderTargetBuffers[m_nSwapChainBufferIndex].Get();
		d3dResourceBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
		d3dResourceBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
		d3dResourceBarrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
		m_pd3dCommandList->ResourceBarrier(1, &d3dResourceBarrier);
		{
			// 랜터타겟뷰 초기화
			D3D12_CPU_DESCRIPTOR_HANDLE d3dRtvCPUDescriptorHandle =
				m_pd3dRtvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
			d3dRtvCPUDescriptorHandle.ptr += 
				(m_nSwapChainBufferIndex * m_nRtvDescriptorIncrementSize);
			float pfClearColor[4] = { 0.0f, 0.125f, 0.3f, 1.0f };
			m_pd3dCommandList->ClearRenderTargetView(
				  d3dRtvCPUDescriptorHandle
				, pfClearColor/*Colors::Azure*/
				, 0
				, NULL);

			// 뎁스스텐실뷰 초기화
			D3D12_CPU_DESCRIPTOR_HANDLE d3dDsvCPUDescriptorHandle =
				m_pd3dDsvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
			m_pd3dCommandList->ClearDepthStencilView(
				  d3dDsvCPUDescriptorHandle
				, D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL
				, 1.0f
				, 0
				, 0
				, NULL);

			// Output merser 설정
			m_pd3dCommandList->OMSetRenderTargets(
				  1
				, &d3dRtvCPUDescriptorHandle
				, TRUE
				, &d3dDsvCPUDescriptorHandle);

			// 렌더링
			if (m_pScene) m_pScene->Render(m_pd3dCommandList.Get(), m_pCamera);

#ifdef _WITH_PLAYER_TOP
			//3인칭 카메라일 때 플레이어가 항상 보이도록 렌더링한다.
			//렌더 타겟은 그대로 두고 깊이 버퍼를 1.0으로 지우고 
			//플레이어를 렌더링하면 플레이어는 무조건 그려질 것이다.
			m_pd3dCommandList->ClearDepthStencilView(
				d3dDsvCPUDescriptorHandle
				, D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL
				, 1.0f
				, 0
				, 0
				, NULL);
#endif
			//3인칭 카메라일 때 플레이어를 렌더링한다.
			if (m_pPlayer) m_pPlayer->Render(m_pd3dCommandList.Get(), m_pCamera);
		}
		d3dResourceBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
		d3dResourceBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
		d3dResourceBarrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
		m_pd3dCommandList->ResourceBarrier(1, &d3dResourceBarrier);
	}
	m_pd3dCommandList->Close();

	ID3D12CommandList *ppd3dCommandLists[] = { m_pd3dCommandList.Get() };
	m_pd3dCommandQueue->ExecuteCommandLists(1, ppd3dCommandLists);

	WaitForGpuComplete();
#ifdef _WITH_PRESENT_PARAMETERS
	DXGI_PRESENT_PARAMETERS dxgiPresentParameters;
	dxgiPresentParameters.DirtyRectsCount = 0;
	dxgiPresentParameters.pDirtyRects = NULL;
	dxgiPresentParameters.pScrollRect = NULL;
	dxgiPresentParameters.pScrollOffset = NULL;
	m_pdxgiSwapChain->Present1(1, 0, &dxgiPresentParameters);
#else
#ifdef _WITH_SYNCH_SWAPCHAIN
	m_pdxgiSwapChain->Present(1, 0);
#else
	m_pdxgiSwapChain->Present(0, 0);
#endif
#endif
	MoveToNextFrame();

	m_GameTimer.GetFrameRate(m_pszFrameRate + 12, 37);
	::SetWindowText(m_hWnd, m_pszFrameRate);
}

void CGameFramework::CreateCommandQueueAndList()
{
	//직접(Direct) 명령 큐를 생성한다.
	m_pd3dDeviceIndRes->CreateCommandQueue(&m_pd3dCommandQueue);

	//직접(Direct) 명령 할당자를 생성한다.
	m_pd3dDeviceIndRes->CreateCommandAllocator(
		  D3D12_COMMAND_LIST_TYPE_DIRECT
		, &m_pd3dCommandAllocator);

	//직접(Direct) 명령 리스트를 생성한다.
	m_pd3dDeviceIndRes->CreateCommandList(
		  0
		, D3D12_COMMAND_LIST_TYPE_DIRECT
		, m_pd3dCommandAllocator.Get()
		, NULL
		, &m_pd3dCommandList);

	//명령 리스트는 생성되면 열린(Open) 상태이므로 닫힌(Closed) 상태로 만든다.
	m_pd3dCommandList->Close();
}
void CGameFramework::CreateSwapChain()
{
	m_pd3dDeviceIndRes->CreateSwapChain(
		  m_hWnd
		, m_nSwapChainBuffers
		, m_nWndClientWidth
		, m_nWndClientHeight
		, m_pd3dCommandQueue.Get()
		, (IDXGISwapChain**)m_pdxgiSwapChain.GetAddressOf());

	//“Alt+Enter” 키의 동작을 비활성화한다.
	m_pd3dDeviceIndRes->MakeWindowAssociation(m_hWnd, DXGI_MWA_NO_ALT_ENTER);

	//스왑체인의 현재 후면버퍼 인덱스를 저장한다.
	m_nSwapChainBufferIndex = m_pdxgiSwapChain->GetCurrentBackBufferIndex();
}
void CGameFramework::CreateRtvAndDsvDescriptorHeaps()
{
	//렌더 타겟 서술자 힙(서술자의 개수는 스왑체인 버퍼의 개수)을 생성한다.
	m_pd3dDeviceIndRes->CreateDescriptorHeap(
		D3D12_DESCRIPTOR_HEAP_TYPE_RTV
		, m_nSwapChainBuffers
		, D3D12_DESCRIPTOR_HEAP_FLAG_NONE
		, 0
		, &m_pd3dRtvDescriptorHeap);
	//렌더 타겟 서술자 힙의 원소의 크기를 저장한다.
	m_nRtvDescriptorIncrementSize =
		m_pd3dDeviceIndRes->GetDescriptorHandleIncrementSize(
			D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

	//깊이-스텐실 서술자 힙(서술자의 개수는 1)을 생성한다.
	m_pd3dDeviceIndRes->CreateDescriptorHeap(
		  D3D12_DESCRIPTOR_HEAP_TYPE_DSV
		, 1
		, D3D12_DESCRIPTOR_HEAP_FLAG_NONE
		, 0
		, &m_pd3dDsvDescriptorHeap);
	//깊이-스텐실 서술자 힙의 원소의 크기를 저장한다.
	m_nDsvDescriptorIncrementSize =
		m_pd3dDeviceIndRes->GetDescriptorHandleIncrementSize(
			D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
}
void CGameFramework::CreateRenderTargetView()
{
	//스왑체인의 각 후면 버퍼에 대한 렌더 타겟 뷰를 생성한다.
	D3D12_CPU_DESCRIPTOR_HANDLE d3dRtvCPUDescriptorHandle =
		m_pd3dRtvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
	for (UINT i = 0; i < m_nSwapChainBuffers; i++)
	{
		m_pdxgiSwapChain->GetBuffer(
			i, IID_PPV_ARGS(&m_pd3dRenderTargetBuffers[i]));
		m_pd3dDeviceIndRes->CreateRenderTargetView(
			  m_pd3dRenderTargetBuffers[i].Get()
			, NULL
			, d3dRtvCPUDescriptorHandle);
		d3dRtvCPUDescriptorHandle.ptr += m_nRtvDescriptorIncrementSize;
	}
}
void CGameFramework::CreateDepthStencilView()
{
	//깊이-스텐실 버퍼를 생성한다.
	m_pd3dDeviceIndRes->CreateDepthStencilBuffer(
		  m_nWndClientWidth
		, m_nWndClientHeight
		, &m_pd3dDepthStencilBuffer);

	//깊이-스텐실 버퍼 뷰를 생성한다.
	D3D12_CPU_DESCRIPTOR_HANDLE d3dDsvCPUDescriptorHandle =
		m_pd3dDsvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
	m_pd3dDeviceIndRes->CreateDepthStencilView(
		  m_pd3dDepthStencilBuffer.Get()
		, NULL
		, d3dDsvCPUDescriptorHandle);
}

void CGameFramework::BuildObjects()
{
	m_pd3dCommandList->Reset(m_pd3dCommandAllocator.Get(), NULL);
	{
		m_pScene = new CScene();
		if (m_pScene)
		{
			m_pScene->SetHWNDAndHInstance(m_hWnd, m_hInstance);
			m_pScene->BuildObjects(m_pd3dDeviceIndRes, m_pd3dCommandList.Get());
		}

		m_pPlayer = new CAirplanePlayer(
			m_pd3dDeviceIndRes
			, m_pd3dCommandList.Get()
			, m_pScene->GetGraphicsRootSignature()
			, m_pScene->GetTerrain(), 1);
		m_pScene->SetPlayer(m_pPlayer);
		m_pCamera = m_pPlayer->GetCamera();
	}
	m_pd3dCommandList->Close();

	ID3D12CommandList *ppd3dCommandLists[] = { m_pd3dCommandList.Get() };
	m_pd3dCommandQueue->ExecuteCommandLists(1, ppd3dCommandLists);
	WaitForGpuComplete();

	if (m_pScene) 
		m_pScene->ReleaseUploadBuffers();
	m_GameTimer.Reset();
}
void CGameFramework::ReleaseObjects()
{
	if (m_pScene)
	{
		m_pScene->ReleaseObjects();
		delete m_pScene;
	}
	if (m_pPlayer) 
		delete m_pPlayer;
}

void CGameFramework::ProcessInput()
{
	if (m_pScene)
		m_pScene->ProcessInput(m_GameTimer.GetTimeElapsed());
	
}
void CGameFramework::AnimateObjects()
{
	if (m_pScene) 
		m_pScene->AnimateObjects(m_GameTimer.GetTimeElapsed());
}

void CGameFramework::MoveToNextFrame()
{
	m_nSwapChainBufferIndex = m_pdxgiSwapChain->GetCurrentBackBufferIndex();
	WaitForGpuComplete();
}
void CGameFramework::WaitForGpuComplete()
{
	UINT64 nFenceValue = ++m_nFenceValues[m_nSwapChainBufferIndex];
	//GPU가 펜스의 값을 설정하는 명령을 명령 큐에 추가한다.
	m_pd3dCommandQueue->Signal(m_pd3dFence.Get(), nFenceValue);
	if (m_pd3dFence->GetCompletedValue() < nFenceValue)
	{
		//펜스의 현재 값이 설정한 값보다 작으면 펜스의 현재 값이 설정한 값이 될 때까지 기다린다.
		m_pd3dFence->SetEventOnCompletion(nFenceValue, m_hFenceEvent);
		::WaitForSingleObject(m_hFenceEvent, INFINITE);
	}
}

void CGameFramework::ToggleFullScreen()
{
	BOOL bFullScreenState = FALSE;
	m_pdxgiSwapChain->GetFullscreenState(&bFullScreenState, NULL);
	if (!bFullScreenState)
	{
		DXGI_MODE_DESC dxgiTargetParameters;
		dxgiTargetParameters.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		dxgiTargetParameters.Width = m_nWndClientWidth;
		dxgiTargetParameters.Height = m_nWndClientHeight;
		dxgiTargetParameters.RefreshRate.Numerator = 60;
		dxgiTargetParameters.RefreshRate.Denominator = 1;
		dxgiTargetParameters.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
		dxgiTargetParameters.ScanlineOrdering =
			DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
		m_pdxgiSwapChain->ResizeTarget(&dxgiTargetParameters);
	}
	m_pdxgiSwapChain->SetFullscreenState(!bFullScreenState, NULL);
	OnResizeBackBuffers();
}
