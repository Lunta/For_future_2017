#pragma once

#include "Scene/Scene.h"
#include "Camera/Camera.h"
#include "Object/Player/Player.h"
#include "Object/Item/Item.h"
#include "UI/Inventory/Inventory.h"
class CTestScene :
	public CScene
{
public:
	using Base = CScene;
public:
	CTestScene();
	~CTestScene();

	bool OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)	override;
	bool OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)		override;
	bool OnProcessingWindowMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)	override;

	bool OnCreate(wstring&& tag, CWarp2DFramework* pFramework) override;
	void Update(float fTimeElapsed) override;
	void Draw(ID2D1HwndRenderTarget * pd2dRenderTarget) override;

private:

	ComPtr<ID2D1SolidColorBrush>	m_pd2dsbrDefault	{ nullptr }	;
	ComPtr<ID2D1SolidColorBrush>	m_pd2dsbrTileA		{ nullptr }	;
	ComPtr<ID2D1SolidColorBrush>	m_pd2dsbrTileB		{ nullptr }	;


	CPlayer							m_Player;
	CCamera							m_Camera;

	list<unique_ptr<CItem>>			m_lstItem;
	list<unique_ptr<CItem>>			m_lstTrap;
	
	CUIInventory					m_uiInventory;
};
