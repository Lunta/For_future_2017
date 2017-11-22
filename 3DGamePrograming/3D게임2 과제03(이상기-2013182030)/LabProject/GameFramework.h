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
	HINSTANCE							m_hInstance;
	HWND								m_hWnd;

	int									m_nWndClientWidth;
	int									m_nWndClientHeight;

	CGameTimer							m_GameTimer;

	CScene								*m_pScene;
	CCamera								*m_pCamera;
	CPlayer								*m_pPlayer;
	CGameObject							*m_pSelectedObject = NULL;

	POINT								m_ptOldCursorPos;
	_TCHAR								m_pszFrameRate[50];

private:
	CD3DDeviceIndRes					*m_pd3dDeviceIndRes;

	ComPtr<ID3D12CommandQueue>			m_pd3dCommandQueue;
	ComPtr<ID3D12CommandAllocator>		m_pd3dCommandAllocator;
	ComPtr<ID3D12GraphicsCommandList>	m_pd3dCommandList;

	ComPtr<IDXGISwapChain3>				m_pdxgiSwapChain;
	static const UINT					m_nSwapChainBuffers = NUM_OF_SWAP_CHAIN_BUFFERS;
	UINT								m_nSwapChainBufferIndex;

	ComPtr<ID3D12DescriptorHeap>		m_pd3dRtvDescriptorHeap;
	UINT								m_nRtvDescriptorIncrementSize;
	ComPtr<ID3D12Resource>				m_pd3dRenderTargetBuffers[m_nSwapChainBuffers];

	ComPtr<ID3D12DescriptorHeap>		m_pd3dDsvDescriptorHeap;
	UINT								m_nDsvDescriptorIncrementSize;
	ComPtr<ID3D12Resource>				m_pd3dDepthStencilBuffer;

	ComPtr<ID3D12Fence>					m_pd3dFence;
	UINT64								m_nFenceValues[m_nSwapChainBuffers];
	HANDLE								m_hFenceEvent;

private:
	void CreateCommandQueueAndList();
	void CreateSwapChain();
	void CreateRtvAndDsvDescriptorHeaps();
	void CreateRenderTargetView();
	void CreateDepthStencilView();

	void BuildObjects();
	void ReleaseObjects();

	void ProcessInput();
	void AnimateObjects();

	void MoveToNextFrame();
	void WaitForGpuComplete();

	void ToggleFullScreen();
};
#endif