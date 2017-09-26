#pragma once

//-----------------------------------------------------------------------------
// File: GameFramework.h
//-----------------------------------------------------------------------------

#ifndef _GAME_APPLICATION_FRAMEWORK_H_
#define _GAME_APPLICATION_FRAMEWORK_H_

#include "Scene.h"
#include "GameTimer.h"
#include "Camera.h"
#include "Player.h"

#define NUM_OF_SWAP_CHAIN_BUFFERS 2

class CGameFramework
{
public:
	CGameFramework();
	~CGameFramework();

public:
	bool OnCreate(HINSTANCE hInstance, HWND hMainWnd);
	void OnDestroy();
	void OnResizeBackBuffers();

	void OnProcessingMouseMessage(
		HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	void OnProcessingKeyboardMessage(
		HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	LRESULT CALLBACK OnProcessingWindowMessage(
		HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);

	void FrameAdvance();

	CPlayer* GetPlayer() const { return m_pPlayer; }

private:
	HINSTANCE					m_hInstance;
	HWND						m_hWnd;

	CScene						*m_pScene;
	CCamera						*m_pCamera;
	CPlayer						*m_pPlayer;
	CGameObject					*m_pSelectedObject = NULL;

	CGameTimer					m_GameTimer;

	POINT						m_ptOldCursorPos;

	//다음은 프레임 레이트를 주 윈도우의 캡션에 출력하기 위한 문자열이다.
	_TCHAR						m_pszFrameRate[50];

#if defined(_DEBUG)
	// 디버그 컨트롤러
	ComPtr<ID3D12Debug>					m_pd3dDebugController;
#endif

	// DXGI 팩토리 인터페이스에 대한 포인터이다.
	ComPtr<IDXGIFactory4>				m_pdxgiFactory;

	// Direct3D 디바이스 인터페이스에 대한 포인터이다. 주로 리소스를 생성하기 위하여 필요하다.
	ComPtr<ID3D12Device>				m_pd3dDevice;

	// MSAA 다중 샘플링을 활성화하고 다중 샘플링 레벨을 설정한다.
	bool								m_bMsaa4xEnable;
	UINT								m_nMsaa4xQualityLevels;

	// 명령 큐, 명령 할당자, 명령 리스트 인터페이스 포인터이다.
	ComPtr<ID3D12CommandQueue>			m_pd3dCommandQueue;
	ComPtr<ID3D12CommandAllocator>		m_pd3dCommandAllocator;
	ComPtr<ID3D12GraphicsCommandList>	m_pd3dCommandList;

	// 스왑 체인 인터페이스에 대한 포인터이다. 주로 디스플레이를 제어하기 위하여 필요하다.
	int									m_nWndClientWidth;
	int									m_nWndClientHeight;
	ComPtr<IDXGISwapChain3>				m_pdxgiSwapChain;

	// 스왑 체인의 후면 버퍼의 개수와 인덱스이다.
	static const UINT					m_nSwapChainBuffers = NUM_OF_SWAP_CHAIN_BUFFERS;
	UINT								m_nSwapChainBufferIndex;

	// 렌더 타겟 버퍼, 서술자 힙 인터페이스 포인터, 렌더 타겟 서술자 원소의 크기이다.
	ComPtr<ID3D12DescriptorHeap>		m_pd3dRtvDescriptorHeap;
	UINT								m_nRtvDescriptorIncrementSize;
	ComPtr<ID3D12Resource>				m_pd3dRenderTargetBuffers[m_nSwapChainBuffers];

	// 깊이-스텐실 버퍼, 서술자 힙 인터페이스 포인터, 깊이-스텐실 서술자 원소의 크기이다.
	ComPtr<ID3D12DescriptorHeap>		m_pd3dDsvDescriptorHeap;
	UINT								m_nDsvDescriptorIncrementSize;
	ComPtr<ID3D12Resource>				m_pd3dDepthStencilBuffer;

	// 펜스 인터페이스 포인터, 펜스의 값, 이벤트 핸들이다.
	ComPtr<ID3D12Fence>					m_pd3dFence;
	UINT64								m_nFenceValues[m_nSwapChainBuffers];
	HANDLE								m_hFenceEvent;

private:
	void CreateDirect3DDevice();
	void CreateCommandQueueAndList();
	void CreateSwapChain();
	void CreateRtvAndDsvDescriptorHeaps();
	void CreateRenderTargetView();
	void CreateDepthStencilView();

	void BuildObjects();
	void ReleaseObjects();

	void ProcessInput();
	void AnimateObjects();

	// CPU와 GPU를 동기화하는 함수
	void MoveToNextFrame();
	void WaitForGpuComplete();
};
#endif