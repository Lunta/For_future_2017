#include "stdafx.h"

#include "Framework/Warp2DFramework.h"
#include "Framework/IndRes/IndRes.h"

#include "TestScene.h"


CTestScene::CTestScene()
{
}
CTestScene::~CTestScene()
{
}

bool CTestScene::OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	switch (nMessageID)
	{
	case WM_LBUTTONDOWN:	
	case WM_MBUTTONDOWN:	
	case WM_RBUTTONDOWN:	
	case WM_MOUSEMOVE:		
	case WM_LBUTTONUP:		
	case WM_MBUTTONUP:		
	case WM_RBUTTONUP:		
	case WM_MOUSEWHEEL:		
		break;
	default:
		return false;
	}

	return(true);
}

bool CTestScene::OnProcessingWindowMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	switch (nMessageID)
	{
	case WM_SIZE:
	{
		auto rcClient = m_pFramework->GetClientSize();
		m_Camera.SetClientSize(Point2F(rcClient.right, rcClient.bottom));
		break;
	}
	case WM_LBUTTONDOWN:
	case WM_MBUTTONDOWN:
	case WM_RBUTTONDOWN:
	case WM_MOUSEMOVE:
	case WM_LBUTTONUP:
	case WM_MBUTTONUP:
	case WM_RBUTTONUP:
	case WM_MOUSEWHEEL:
		return OnProcessingMouseMessage(hWnd, nMessageID, wParam, lParam);

	case WM_KEYDOWN:
	case WM_KEYUP:
	case WM_CHAR:
		return OnProcessingKeyboardMessage(hWnd, nMessageID, wParam, lParam);

	default:
		return false;
	}

	return true;
}

bool CTestScene::OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	switch (nMessageID)
	{
		
	case WM_KEYDOWN:
		switch (wParam)
		{
		case 'A': m_Player.Move(-10.f, 0);
			break;				 
		case 'W': m_Player.Move(0, -10.f);
			break;				 
		case 'D': m_Player.Move(10.f, 0);
			break;				 
		case 'S': m_Player.Move(0, 10.f);
			break;
		case 'Z': m_Camera.Scale(m_Camera.GetScale() * 2.f);
			break;
		case 'X': m_Camera.Scale(m_Camera.GetScale() * 0.5f);
			break;
		case 'G': //m_uiInventory.SetItem(m_upItem.get());
			break;
		case 'H': //m_uiInventory.SetItem(nullptr);
			break;
		default:
			return false;
		}
		break;
	default:
		return false;
	}
	return(true);
}

bool CTestScene::OnCreate(wstring && tag, CWarp2DFramework * pFramework)
{
	if (!Base::OnCreate(std::move(tag), pFramework)) return false;

	auto rcClient = pFramework->GetClientSize();
	m_Camera.SetClientSize(Point2F(rcClient.right, rcClient.bottom));
	auto rendertarget = pFramework->GetRenderTarget();

	rendertarget->CreateSolidColorBrush(ColorF{ ColorF::Crimson }, &m_pd2dsbrDefault);
	rendertarget->CreateSolidColorBrush(ColorF{ ColorF::Green }, &m_pd2dsbrGrid1);
	rendertarget->CreateSolidColorBrush(ColorF{ ColorF::GreenYellow }, &m_pd2dsbrGrid2);

	m_Camera.SetPosition(m_Player.GetPos());
	m_Camera.SetAnchor(Point2F(0.0f, 0.0f));

	m_upItem = make_unique<CItem>(
		Point2F(100, 100)
		, RectF(-10, -10, 10, 10));
	m_upItem->RegisterImage(
		m_pIndRes.get()
		, rendertarget.Get()
		, "Assets/Icon/Buckler.png");

	m_uiInventory.BuildObject(this);
	m_Player.RegisterSpriteImage(
		m_pIndRes.get()
		, rendertarget.Get()
		, "Assets/player.png"
		, Point2F(4, 4));
	//auto dwFactoy = m_pIndRes->dwFactory();
	//
	//dwFactoy->CreateTextFormat(
	//  L"Arial"
	//, nullptr
	//, DWRITE_FONT_WEIGHT_NORMAL
	//, DWRITE_FONT_STYLE_NORMAL
	//, DWRITE_FONT_STRETCH_NORMAL
	//, 30
	//, L"ko-kr"
	//, &m_pdwTextFormat);
	//
	//m_pdwTextFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
	//m_pdwTextFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
	//
	//dwFactoy->CreateTextLayout(
	//	  L"Hello, World!"
	//	, 13
	//	, m_pdwTextFormat.Get()
	//	, 300
	//	, 100
	//	, &m_pdwTextLayout);

	return true;
}

void CTestScene::Update(float fTimeElapsed)
{
	m_Camera.SetPosition(m_Player.GetPos());
	m_upItem->Update(fTimeElapsed);
	m_uiInventory.Update(fTimeElapsed);
	m_Player.Update(fTimeElapsed);
}

void CTestScene::Draw(ID2D1HwndRenderTarget * pd2dRenderTarget)
{
	auto cameramtx = m_Camera.RegenerateViewMatrix();
	pd2dRenderTarget->SetTransform(cameramtx);

	for (int i = -50; i < 50; ++i)
		for (int j = -50; j < 50; ++j)
		{
			if ((i + j) % 2)
				pd2dRenderTarget->FillRectangle(
					RectF(-10, -10, 10, 10) + 
					Point2F(20.f*j, 20.f*i)
					, m_pd2dsbrGrid1.Get());
			else
				pd2dRenderTarget->FillRectangle(
					RectF(-10, -10, 10, 10) + 
					Point2F(20.f*j, 20.f*i)
					, m_pd2dsbrGrid2.Get());
		}

	m_Player.Draw(pd2dRenderTarget);
	pd2dRenderTarget->DrawRectangle(
		RectF(60, 60, 80, 80)
		, m_pd2dsbrDefault.Get());

	m_upItem->Draw(pd2dRenderTarget);
	m_uiInventory.Draw(pd2dRenderTarget);

	//pd2dRenderTarget->DrawTextLayout(
	//	  Point2F(0, 0)
	//	, m_pdwTextLayout.Get()
	//	, m_pd2dsbrDefault.Get()
	//);

}
