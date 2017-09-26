//-----------------------------------------------------------------------------
// File: CGameFramework.cpp
//-----------------------------------------------------------------------------

#include "stdafx.h"
#include "Scene.h"
#include "Camera.h"
#include "Player.h"
#include "GameFramework.h"

CGameFramework::CGameFramework()
{
	m_hInstance = NULL;
	m_hWnd = NULL;     

	m_pScene = NULL;
	m_pCamera = NULL;
	m_pPlayer = NULL;

	m_ptOldCursorPos = { 0, 0 };

	_tcscpy_s(m_pszFrameRate, _T("3D����1 ����02(�̻��-2013182030)|("));

#if defined(_DEBUG)
	// ����� ��Ʈ�ѷ�
	m_pd3dDebugController = NULL;
#endif

	// DXGI ���丮 �������̽��� ���� �������̴�.
	m_pdxgiFactory = NULL;

	// Direct3D ����̽� �������̽��� ���� �������̴�. �ַ� ���ҽ��� �����ϱ� ���Ͽ� �ʿ��ϴ�.
	m_pd3dDevice = NULL;

	// MSAA ���� ���ø��� Ȱ��ȭ�ϰ� ���� ���ø� ������ �����Ѵ�.
	m_bMsaa4xEnable = false;
	m_nMsaa4xQualityLevels = 0;

	// ��� ť, ��� �Ҵ���, ��� ����Ʈ �������̽� �������̴�.
	m_pd3dCommandQueue = NULL;
	m_pd3dCommandAllocator = NULL;
	m_pd3dCommandList = NULL;

	// ���� ü�� �������̽��� ���� �������̴�. �ַ� ���÷��̸� �����ϱ� ���Ͽ� �ʿ��ϴ�.
	m_nWndClientWidth = FRAME_BUFFER_WIDTH;
	m_nWndClientHeight = FRAME_BUFFER_HEIGHT;
	m_pdxgiSwapChain = NULL;

	// ���� ü���� �ĸ� ������ �ε����̴�.
	m_nSwapChainBufferIndex = 0;

	// ���� Ÿ�� ����, ������ �� �������̽� ������, ���� Ÿ�� ������ ������ ũ���̴�.
	m_pd3dRtvDescriptorHeap = NULL;
	m_nRtvDescriptorIncrementSize = 0;
	for (int i = 0; i < m_nSwapChainBuffers; i++) 
		m_pd3dRenderTargetBuffers[i] = NULL;

	// ����-���ٽ� ����, ������ �� �������̽� ������, ����-���ٽ� ������ ������ ũ���̴�.
	m_nDsvDescriptorIncrementSize = 0;
	m_pd3dDsvDescriptorHeap = NULL;
	m_pd3dDepthStencilBuffer = NULL;

	// �潺 �������̽� ������, �潺�� ��, �̺�Ʈ �ڵ��̴�.
	m_pd3dFence = NULL;
	for (int i = 0; i < m_nSwapChainBuffers; i++) 
		m_nFenceValues[i] = 0;
	m_hFenceEvent = NULL;
}
CGameFramework::~CGameFramework()
{
}

bool CGameFramework::OnCreate(HINSTANCE hInstance, HWND hMainWnd)
{
#ifdef _DEBUG
	system("mode con:cols=50 lines=30");
	system("title Debug Console");
#endif

    srand((unsigned int)time(NULL));

	m_hInstance = hInstance;
	m_hWnd = hMainWnd;

	//Direct3D ����̽�, ��� ť�� ��� ����Ʈ, ���� ü�� ���� �����ϴ� �Լ��� ȣ���Ѵ�.
	CreateDirect3DDevice();
	CreateCommandQueueAndList();
	CreateSwapChain();
	CreateRtvAndDsvDescriptorHeaps();

	m_hFenceEvent = ::CreateEvent(NULL, FALSE, FALSE, NULL);

	BuildObjects();

	return(true);
}
void CGameFramework::OnDestroy()
{
	ReleaseObjects();

	if (m_hWnd) DestroyWindow(m_hWnd);

#if defined(_DEBUG)
	if (m_pd3dDebugController)		m_pd3dDebugController->Release();
#endif
	if (m_pdxgiFactory)				m_pdxgiFactory->Release();

	if (m_pd3dDevice)				m_pd3dDevice->Release();

	if (m_pd3dCommandQueue)			m_pd3dCommandQueue->Release();
	if (m_pd3dCommandAllocator)		m_pd3dCommandAllocator->Release();
	if (m_pd3dCommandList)			m_pd3dCommandList->Release();

	m_pdxgiSwapChain->SetFullscreenState(FALSE, NULL);
	if (m_pdxgiSwapChain)			m_pdxgiSwapChain->Release();

	if (m_pd3dRtvDescriptorHeap)	m_pd3dRtvDescriptorHeap->Release();
	for (int i = 0; i < m_nSwapChainBuffers; i++) 
		if (m_pd3dRenderTargetBuffers[i])
			m_pd3dRenderTargetBuffers[i]->Release();

	if (m_pd3dDsvDescriptorHeap)	m_pd3dDsvDescriptorHeap->Release();
	if (m_pd3dDepthStencilBuffer)	m_pd3dDepthStencilBuffer->Release();

	if (m_pd3dFence)				m_pd3dFence->Release();
	::CloseHandle(m_hFenceEvent);
}
void CGameFramework::OnResizeBackBuffers()
{
	WaitForGpuComplete();
	m_pd3dCommandList->Reset(m_pd3dCommandAllocator, NULL);
	{
		for (int i = 0; i < m_nSwapChainBuffers; i++)
		{
			if (m_pd3dRenderTargetBuffers[i])
				m_pd3dRenderTargetBuffers[i]->Release();
		}
		if (m_pd3dDepthStencilBuffer) 
			m_pd3dDepthStencilBuffer->Release();

#ifdef _WITH_ONLY_RESIZE_BACKBUFFERS
		DXGI_SWAP_CHAIN_DESC dxgiSwapChainDesc;
		m_pdxgiSwapChain->GetDesc(&dxgiSwapChainDesc);
		m_pdxgiSwapChain->ResizeBuffers(0, 0, 0, DXGI_FORMAT_UNKNOWN, 0);
		m_nSwapChainBufferIndex = 0;
#else
		DXGI_SWAP_CHAIN_DESC dxgiSwapChainDesc;
		m_pdxgiSwapChain->GetDesc(&dxgiSwapChainDesc);
		m_pdxgiSwapChain->ResizeBuffers(m_nSwapChainBuffers, m_nWndClientWidth,
			m_nWndClientHeight, dxgiSwapChainDesc.BufferDesc.Format, dxgiSwapChainDesc.Flags);
		m_nSwapChainBufferIndex = 0;
#endif

		CreateRenderTargetView();
		CreateDepthStencilView();
	}
	m_pd3dCommandList->Close();

	ID3D12CommandList *ppd3dCommandLists[] = { m_pd3dCommandList };
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
			//���콺 ĸ�ĸ� �ϰ� ���� ���콺 ��ġ�� �����´�.
			::SetCapture(hWnd);
			::GetCursorPos(&m_ptOldCursorPos);
			break;
		case WM_LBUTTONUP:
		case WM_RBUTTONUP:
			//���콺 ĸ�ĸ� �����Ѵ�.
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
			/* ��F1�� Ű�� ������ 1��Ī ī�޶�, 
			// ��F2�� Ű�� ������ �����̽�-�� ī�޶�� �����Ѵ�, 
			// ��F3�� Ű�� ������ 3��Ī ī�޶� �� �����Ѵ�. */
		case VK_F1:
		case VK_F2:
		case VK_F3:
#if CAMERA_MODE_ACTIVE
			if (m_pPlayer) m_pPlayer->ChangeCamera(m_pd3dDevice, (wParam - VK_F1 + 1),
				m_GameTimer.GetTimeElapsed());
			m_pCamera = m_pPlayer->GetCamera();
#endif
			break;
		case VK_ESCAPE:
			::PostQuitMessage(0);
			break;
		case VK_RETURN:
			break;
			//��F9�� Ű�� �������� ������ ���� ��üȭ�� ����� ��ȯ�� ó���Ѵ�.
		case VK_F9:
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
			break;
		}
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

	HRESULT hResult = m_pd3dCommandAllocator->Reset();
	hResult = m_pd3dCommandList->Reset(m_pd3dCommandAllocator, NULL);
	{
		D3D12_RESOURCE_BARRIER d3dResourceBarrier;
		::ZeroMemory(&d3dResourceBarrier, sizeof(D3D12_RESOURCE_BARRIER));
		d3dResourceBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		d3dResourceBarrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
		d3dResourceBarrier.Transition.pResource =
			m_pd3dRenderTargetBuffers[m_nSwapChainBufferIndex];
		d3dResourceBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
		d3dResourceBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
		d3dResourceBarrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
		m_pd3dCommandList->ResourceBarrier(1, &d3dResourceBarrier);
		{
			// ����Ÿ�ٺ� �ʱ�ȭ
			D3D12_CPU_DESCRIPTOR_HANDLE d3dRtvCPUDescriptorHandle =
				m_pd3dRtvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
			d3dRtvCPUDescriptorHandle.ptr += 
				(m_nSwapChainBufferIndex * m_nRtvDescriptorIncrementSize);
			float pfClearColor[4] = { 0.0f, 0.125f, 0.3f, 1.0f };
			m_pd3dCommandList->ClearRenderTargetView(
				d3dRtvCPUDescriptorHandle, pfClearColor/*Colors::Azure*/, 0, NULL);

			// �������ٽǺ� �ʱ�ȭ
			D3D12_CPU_DESCRIPTOR_HANDLE d3dDsvCPUDescriptorHandle =
				m_pd3dDsvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
			m_pd3dCommandList->ClearDepthStencilView(d3dDsvCPUDescriptorHandle,
				D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, NULL);

			// Output merser ����
			m_pd3dCommandList->OMSetRenderTargets(1, &d3dRtvCPUDescriptorHandle, TRUE,
				&d3dDsvCPUDescriptorHandle);

			// ������
			if (m_pScene) m_pScene->Render(m_pd3dCommandList, m_pCamera);

#ifdef _WITH_PLAYER_TOP
			//3��Ī ī�޶��� �� �÷��̾ �׻� ���̵��� �������Ѵ�.
			//���� Ÿ���� �״�� �ΰ� ���� ���۸� 1.0���� ����� 
			//�÷��̾ �������ϸ� �÷��̾�� ������ �׷��� ���̴�.
			m_pd3dCommandList->ClearDepthStencilView(d3dDsvCPUDescriptorHandle,
				D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, NULL);
#endif
			//3��Ī ī�޶��� �� �÷��̾ �������Ѵ�.
			if (m_pPlayer) if(m_pPlayer->IsActive())
				m_pPlayer->Render(m_pd3dCommandList, m_pCamera);
		}
		d3dResourceBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
		d3dResourceBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
		d3dResourceBarrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
		m_pd3dCommandList->ResourceBarrier(1, &d3dResourceBarrier);
	}
	hResult = m_pd3dCommandList->Close();

	ID3D12CommandList *ppd3dCommandLists[] = { m_pd3dCommandList };
	m_pd3dCommandQueue->ExecuteCommandLists(1, ppd3dCommandLists);

	WaitForGpuComplete();
	m_pdxgiSwapChain->Present(0, 0);
	MoveToNextFrame();

	m_GameTimer.GetFrameRate(m_pszFrameRate + 28, 21);
	::SetWindowText(m_hWnd, m_pszFrameRate);
}

void CGameFramework::CreateDirect3DDevice()
{
#if defined(_DEBUG)
	D3D12GetDebugInterface(__uuidof(ID3D12Debug), (void **)&m_pd3dDebugController);
	m_pd3dDebugController->EnableDebugLayer();
#endif
	//DXGI ���丮�� �����Ѵ�.
	::CreateDXGIFactory1(__uuidof(IDXGIFactory4), (void **)&m_pdxgiFactory);

	//��� �ϵ���� ����� ���Ͽ� Ư�� ���� 12.0�� �����ϴ� �ϵ���� ����̽��� �����Ѵ�.
	IDXGIAdapter1 *pd3dAdapter = NULL;
	for (UINT i = 0; m_pdxgiFactory->EnumAdapters1(i, &pd3dAdapter) != 
		DXGI_ERROR_NOT_FOUND; ++i)
	{
		DXGI_ADAPTER_DESC1 dxgiAdapterDesc;
		pd3dAdapter->GetDesc1(&dxgiAdapterDesc);

		if (dxgiAdapterDesc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
			continue;

		if (SUCCEEDED(D3D12CreateDevice(pd3dAdapter, D3D_FEATURE_LEVEL_12_0,
				__uuidof(ID3D12Device), (void **)&m_pd3dDevice)))
			break;
	}

	//Ư�� ���� 12.0�� �����ϴ� �ϵ���� ����̽��� ������ �� ������ WARP ����̽��� �����Ѵ�.
	if (!pd3dAdapter)
	{
		m_pdxgiFactory->EnumWarpAdapter(__uuidof(IDXGIFactory4), (void **)&pd3dAdapter);
		D3D12CreateDevice(pd3dAdapter, D3D_FEATURE_LEVEL_11_0,
			__uuidof(ID3D12Device), (void**)&m_pd3dDevice);
	}

	//����̽��� �����ϴ� ���� ������ ǰ�� ������ Ȯ���Ѵ�.
	D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS d3dMsaaQualityLevels;
	d3dMsaaQualityLevels.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	d3dMsaaQualityLevels.SampleCount = 4; //Msaa4x ���� ���ø�
	d3dMsaaQualityLevels.Flags = D3D12_MULTISAMPLE_QUALITY_LEVELS_FLAG_NONE;
	d3dMsaaQualityLevels.NumQualityLevels = 0;

	m_pd3dDevice->CheckFeatureSupport(D3D12_FEATURE_MULTISAMPLE_QUALITY_LEVELS,
		&d3dMsaaQualityLevels, sizeof(D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS));
	m_nMsaa4xQualityLevels = d3dMsaaQualityLevels.NumQualityLevels;

	//���� ������ ǰ�� ������ 1���� ũ�� ���� ���ø��� Ȱ��ȭ�Ѵ�.
	m_bMsaa4xEnable = (m_nMsaa4xQualityLevels > 1) ? true : false;

	m_pd3dDevice->CreateFence(
		0, D3D12_FENCE_FLAG_NONE, __uuidof(ID3D12Fence), (void **)&m_pd3dFence);

	if (pd3dAdapter) pd3dAdapter->Release();
}
void CGameFramework::CreateCommandQueueAndList()
{
	//����(Direct) ��� ť�� �����Ѵ�.
	D3D12_COMMAND_QUEUE_DESC d3dCommandQueueDesc;
	::ZeroMemory(&d3dCommandQueueDesc, sizeof(D3D12_COMMAND_QUEUE_DESC));
	d3dCommandQueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	d3dCommandQueueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
	HRESULT hResult = m_pd3dDevice->CreateCommandQueue(&d3dCommandQueueDesc,
		__uuidof(ID3D12CommandQueue), (void **)&m_pd3dCommandQueue);

	//����(Direct) ��� �Ҵ��ڸ� �����Ѵ�.
	hResult = m_pd3dDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT,
		__uuidof(ID3D12CommandAllocator), (void **)&m_pd3dCommandAllocator);

	//����(Direct) ��� ����Ʈ�� �����Ѵ�.
	hResult = m_pd3dDevice->CreateCommandList(
		0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_pd3dCommandAllocator, NULL, 
		__uuidof(ID3D12GraphicsCommandList), (void**)&m_pd3dCommandList);

	//��� ����Ʈ�� �����Ǹ� ����(Open) �����̹Ƿ� ����(Closed) ���·� �����.
	hResult = m_pd3dCommandList->Close();
}
void CGameFramework::CreateSwapChain()
{
	RECT rcClient;
	::GetClientRect(m_hWnd, &rcClient);
	m_nWndClientWidth = rcClient.right - rcClient.left;
	m_nWndClientHeight = rcClient.bottom - rcClient.top;

	//����ü���� �����Ѵ�.
	DXGI_SWAP_CHAIN_DESC dxgiSwapChainDesc;
	::ZeroMemory(&dxgiSwapChainDesc, sizeof(dxgiSwapChainDesc));
	dxgiSwapChainDesc.BufferCount = m_nSwapChainBuffers;
	dxgiSwapChainDesc.BufferDesc.Width = m_nWndClientWidth;
	dxgiSwapChainDesc.BufferDesc.Height = m_nWndClientHeight;
	dxgiSwapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	dxgiSwapChainDesc.BufferDesc.RefreshRate.Numerator = 60;
	dxgiSwapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
	dxgiSwapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	dxgiSwapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	dxgiSwapChainDesc.OutputWindow = m_hWnd;
	dxgiSwapChainDesc.SampleDesc.Count = (m_bMsaa4xEnable) ? 4 : 1;
	dxgiSwapChainDesc.SampleDesc.Quality =
		(m_bMsaa4xEnable) ? (m_nMsaa4xQualityLevels - 1) : 0;
	dxgiSwapChainDesc.Windowed = TRUE;
#ifdef _WITH_ONLY_RESIZE_BACKBUFFERS
	//��üȭ�� ��忡�� ����ȭ���� �ػ󵵸� �ٲ��� �ʰ� �ĸ������ ũ�⸦ ����ȭ�� ũ��� �����Ѵ�.
	dxgiSwapChainDesc.Flags = 0;
#else
	//��üȭ�� ��忡�� ����ȭ���� �ػ󵵸� ����ü��(�ĸ����)�� ũ�⿡ �°� �����Ѵ�.
	dxgiSwapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
#endif
	HRESULT hResult = m_pdxgiFactory->CreateSwapChain(m_pd3dCommandQueue,
		&dxgiSwapChainDesc, (IDXGISwapChain**)&m_pdxgiSwapChain);

	//��Alt+Enter�� Ű�� ������ ��Ȱ��ȭ�Ѵ�.
	m_pdxgiFactory->MakeWindowAssociation(m_hWnd, DXGI_MWA_NO_ALT_ENTER);

	//����ü���� ���� �ĸ���� �ε����� �����Ѵ�.
	m_nSwapChainBufferIndex = m_pdxgiSwapChain->GetCurrentBackBufferIndex();
}
void CGameFramework::CreateRtvAndDsvDescriptorHeaps()
{
	//���� Ÿ�� ������ ��(�������� ������ ����ü�� ������ ����)�� �����Ѵ�.
	D3D12_DESCRIPTOR_HEAP_DESC d3dDescriptorHeapDesc;
	::ZeroMemory(&d3dDescriptorHeapDesc, sizeof(D3D12_DESCRIPTOR_HEAP_DESC));
	d3dDescriptorHeapDesc.NumDescriptors = m_nSwapChainBuffers;
	d3dDescriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	d3dDescriptorHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	d3dDescriptorHeapDesc.NodeMask = 0;
	HRESULT hResult = m_pd3dDevice->CreateDescriptorHeap(&d3dDescriptorHeapDesc,
		__uuidof(ID3D12DescriptorHeap), (void **)&m_pd3dRtvDescriptorHeap);

	//���� Ÿ�� ������ ���� ������ ũ�⸦ �����Ѵ�.
	m_nRtvDescriptorIncrementSize =
		m_pd3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

	//����-���ٽ� ������ ��(�������� ������ 1)�� �����Ѵ�.
	d3dDescriptorHeapDesc.NumDescriptors = 1;
	d3dDescriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	hResult = m_pd3dDevice->CreateDescriptorHeap(&d3dDescriptorHeapDesc,
		__uuidof(ID3D12DescriptorHeap), (void **)&m_pd3dDsvDescriptorHeap);

	//����-���ٽ� ������ ���� ������ ũ�⸦ �����Ѵ�.
	m_nDsvDescriptorIncrementSize =
		m_pd3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
}
void CGameFramework::CreateRenderTargetView()
{
	//����ü���� �� �ĸ� ���ۿ� ���� ���� Ÿ�� �並 �����Ѵ�.
	D3D12_CPU_DESCRIPTOR_HANDLE d3dRtvCPUDescriptorHandle =
		m_pd3dRtvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
	for (UINT i = 0; i < m_nSwapChainBuffers; i++)
	{
		HRESULT hResult = m_pdxgiSwapChain->GetBuffer(
			i, __uuidof(ID3D12Resource), (void**)&m_pd3dRenderTargetBuffers[i]);
		m_pd3dDevice->CreateRenderTargetView(
			m_pd3dRenderTargetBuffers[i], NULL, d3dRtvCPUDescriptorHandle);
		d3dRtvCPUDescriptorHandle.ptr += m_nRtvDescriptorIncrementSize;
	}
}
void CGameFramework::CreateDepthStencilView()
{
	D3D12_RESOURCE_DESC d3dResourceDesc;
	d3dResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	d3dResourceDesc.Alignment = 0;
	d3dResourceDesc.Width = m_nWndClientWidth;
	d3dResourceDesc.Height = m_nWndClientHeight;
	d3dResourceDesc.DepthOrArraySize = 1;
	d3dResourceDesc.MipLevels = 1;
	d3dResourceDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	d3dResourceDesc.SampleDesc.Count = (m_bMsaa4xEnable) ? 4 : 1;
	d3dResourceDesc.SampleDesc.Quality = 
		(m_bMsaa4xEnable) ? (m_nMsaa4xQualityLevels - 1) : 0;
	d3dResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	d3dResourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

	D3D12_HEAP_PROPERTIES d3dHeapProperties;
	::ZeroMemory(&d3dHeapProperties, sizeof(D3D12_HEAP_PROPERTIES));
	d3dHeapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;
	d3dHeapProperties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	d3dHeapProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
	d3dHeapProperties.CreationNodeMask = 1;
	d3dHeapProperties.VisibleNodeMask = 1;

	D3D12_CLEAR_VALUE d3dClearValue;
	d3dClearValue.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	d3dClearValue.DepthStencil.Depth = 1.0f;
	d3dClearValue.DepthStencil.Stencil = 0;

	//����-���ٽ� ���۸� �����Ѵ�.
	m_pd3dDevice->CreateCommittedResource(&d3dHeapProperties, D3D12_HEAP_FLAG_NONE,
		&d3dResourceDesc, D3D12_RESOURCE_STATE_DEPTH_WRITE, &d3dClearValue,
		__uuidof(ID3D12Resource), (void **)&m_pd3dDepthStencilBuffer);

	//����-���ٽ� ���� �並 �����Ѵ�.
	D3D12_CPU_DESCRIPTOR_HANDLE d3dDsvCPUDescriptorHandle =
		m_pd3dDsvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
	m_pd3dDevice->CreateDepthStencilView(m_pd3dDepthStencilBuffer, NULL,
		d3dDsvCPUDescriptorHandle);
}

void CGameFramework::BuildObjects()
{
	m_pd3dCommandList->Reset(m_pd3dCommandAllocator, NULL);
	{
		m_pScene = new CScene();
		if (m_pScene)
		{
			m_pScene->BuildObjects(m_pd3dDevice, m_pd3dCommandList);
			m_pScene->SetHWNDAndHInstance(m_hWnd, m_hInstance);
		}

		m_pPlayer = new CAirplanePlayer(
				m_pd3dDevice, m_pd3dCommandList, m_pScene->GetGraphicsRootSignature());
		m_pScene->SetPlayer(m_pPlayer);
		m_pCamera = m_pPlayer->GetCamera();

	}
	m_pd3dCommandList->Close();

	ID3D12CommandList *ppd3dCommandLists[] = { m_pd3dCommandList };
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
	//GPU�� �潺�� ���� �����ϴ� ����� ��� ť�� �߰��Ѵ�.
	HRESULT hResult = m_pd3dCommandQueue->Signal(m_pd3dFence, nFenceValue);
	if (m_pd3dFence->GetCompletedValue() < nFenceValue)
	{
		//�潺�� ���� ���� ������ ������ ������ �潺�� ���� ���� ������ ���� �� ������ ��ٸ���.
		hResult = m_pd3dFence->SetEventOnCompletion(nFenceValue, m_hFenceEvent);
		::WaitForSingleObject(m_hFenceEvent, INFINITE);
	}
}
