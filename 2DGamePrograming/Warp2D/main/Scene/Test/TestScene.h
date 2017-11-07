#pragma once

#include "Scene/Scene.h"
#include "Camera\Camera.h"
#include "Object\Item\Item.h"

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

	ComPtr<ID2D1SolidColorBrush>	m_pd2dsbrDefault	{ nullptr };
	ComPtr<ID2D1SolidColorBrush>	m_pd2dsbrGrid1		{ nullptr };
	ComPtr<ID2D1SolidColorBrush>	m_pd2dsbrGrid2		{ nullptr }	;
	//ComPtr<IDWriteTextFormat>		m_pdwTextFormat		{ nullptr }	;
	//ComPtr<IDWriteTextLayout>		m_pdwTextLayout		{ nullptr }	;
	//
	//ComPtr<ID2D1Bitmap1>			m_pd2dbmpTest		{ nullptr }	;
	//float							currImg{ 0 };
	//float							fPositionX{ 100.f };
	D2D_POINT_2F					m_ptPlayer{ Point2F(0,0) };
	CCamera							m_Camera;

	unique_ptr<CItem>				m_upItem;
};

