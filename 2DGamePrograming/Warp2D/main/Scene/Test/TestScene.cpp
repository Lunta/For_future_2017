#include "stdafx.h"

#include "Framework/Warp2DFramework.h"
#include "Framework/IndRes/IndRes.h"

#include "TestScene.h"


CTestScene::CTestScene()
	: m_nItems(0)
	, m_ppItems(nullptr)
{
}
CTestScene::~CTestScene()
{
	if (m_ppItems)
	{
		for (int i = 0; i < m_nItems; ++i)
		{
			if(m_ppItems[i])
				delete m_ppItems[i];
		}
		delete[] m_ppItems;
	}
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

	m_nItems = 5;
	m_ppItems = new CItem*[m_nItems];
	for (int i = 0; i < m_nItems; ++i)
		m_ppItems[i] = new CItem(
			  Point2F(-250 + 100*i, 100)
			, RectF(-10, -10, 10, 10));
	m_ppItems[0]->RegisterImage(
		m_pIndRes.get()
		, rendertarget.Get()
		, "Assets/Icon/Buckler.png");
	m_ppItems[1]->RegisterImage(
		m_pIndRes.get()
		, rendertarget.Get()
		, "Assets/Icon/Bastard Sword.png");
	m_ppItems[2]->RegisterImage(
		m_pIndRes.get()
		, rendertarget.Get()
		, "Assets/Icon/Gramr.png");
	m_ppItems[3]->RegisterImage(
		m_pIndRes.get()
		, rendertarget.Get()
		, "Assets/Icon/Round Shield.png");
	m_ppItems[4]->RegisterImage(
		m_pIndRes.get()
		, rendertarget.Get()
		, "Assets/Icon/Kite Shield.png");

	m_uiInventory.BuildObject(this);
	m_uiInventory.ConnectItemList(m_Player.GetpItemList());
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
	if (m_ppItems)
	{
		for(int i =0 ; i < m_nItems; ++i)
			if(m_ppItems[i])
				m_ppItems[i]->Update(fTimeElapsed);
	}
	m_uiInventory.Update(fTimeElapsed);
	m_Player.Update(fTimeElapsed);

	if (m_ppItems)
	{
		D2D1_POINT_2F player_pos = m_Player.GetPos();
		for (int i = 0; i < m_nItems; ++i)
			if (m_ppItems[i])
			{
				if (PtInRect(&(m_ppItems[i]->GetPos() +
					m_ppItems[i]->GetSize()), player_pos))
				{
					if (m_Player.PickUpItem(*m_ppItems[i]))
					{
						delete m_ppItems[i];
						m_ppItems[i] = nullptr;
					}
				}
			}
	}
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
	pd2dRenderTarget->FillRectangle(
		RectF(-10, -10, 10, 10)
		, m_pd2dsbrDefault.Get());

	if(m_ppItems) for (int i = 0; i < m_nItems; ++i)
		if(m_ppItems[i])
			m_ppItems[i]->Draw(pd2dRenderTarget);

	m_uiInventory.Draw(pd2dRenderTarget);

	//pd2dRenderTarget->DrawTextLayout(
	//	  Point2F(0, 0)
	//	, m_pdwTextLayout.Get()
	//	, m_pd2dsbrDefault.Get()
	//);
}
