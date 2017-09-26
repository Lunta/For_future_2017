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

	//������ ������ ����Ʈ�� �� �������� ĸ�ǿ� ����ϱ� ���� ���ڿ��̴�.
	_TCHAR						m_pszFrameRate[50];

#if defined(_DEBUG)
	// ����� ��Ʈ�ѷ�
	ComPtr<ID3D12Debug>					m_pd3dDebugController;
#endif

	// DXGI ���丮 �������̽��� ���� �������̴�.
	ComPtr<IDXGIFactory4>				m_pdxgiFactory;

	// Direct3D ����̽� �������̽��� ���� �������̴�. �ַ� ���ҽ��� �����ϱ� ���Ͽ� �ʿ��ϴ�.
	ComPtr<ID3D12Device>				m_pd3dDevice;

	// MSAA ���� ���ø��� Ȱ��ȭ�ϰ� ���� ���ø� ������ �����Ѵ�.
	bool								m_bMsaa4xEnable;
	UINT								m_nMsaa4xQualityLevels;

	// ��� ť, ��� �Ҵ���, ��� ����Ʈ �������̽� �������̴�.
	ComPtr<ID3D12CommandQueue>			m_pd3dCommandQueue;
	ComPtr<ID3D12CommandAllocator>		m_pd3dCommandAllocator;
	ComPtr<ID3D12GraphicsCommandList>	m_pd3dCommandList;

	// ���� ü�� �������̽��� ���� �������̴�. �ַ� ���÷��̸� �����ϱ� ���Ͽ� �ʿ��ϴ�.
	int									m_nWndClientWidth;
	int									m_nWndClientHeight;
	ComPtr<IDXGISwapChain3>				m_pdxgiSwapChain;

	// ���� ü���� �ĸ� ������ ������ �ε����̴�.
	static const UINT					m_nSwapChainBuffers = NUM_OF_SWAP_CHAIN_BUFFERS;
	UINT								m_nSwapChainBufferIndex;

	// ���� Ÿ�� ����, ������ �� �������̽� ������, ���� Ÿ�� ������ ������ ũ���̴�.
	ComPtr<ID3D12DescriptorHeap>		m_pd3dRtvDescriptorHeap;
	UINT								m_nRtvDescriptorIncrementSize;
	ComPtr<ID3D12Resource>				m_pd3dRenderTargetBuffers[m_nSwapChainBuffers];

	// ����-���ٽ� ����, ������ �� �������̽� ������, ����-���ٽ� ������ ������ ũ���̴�.
	ComPtr<ID3D12DescriptorHeap>		m_pd3dDsvDescriptorHeap;
	UINT								m_nDsvDescriptorIncrementSize;
	ComPtr<ID3D12Resource>				m_pd3dDepthStencilBuffer;

	// �潺 �������̽� ������, �潺�� ��, �̺�Ʈ �ڵ��̴�.
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

	// CPU�� GPU�� ����ȭ�ϴ� �Լ�
	void MoveToNextFrame();
	void WaitForGpuComplete();
};
#endif