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
		case 'A': m_ptPlayer.x -= 10.f;
			break;				 
		case 'W': m_ptPlayer.y -= 10.f;
			break;				 
		case 'D': m_ptPlayer.x += 10.f;
			break;				 
		case 'S': m_ptPlayer.y += 10.f;
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

	m_Camera.SetPosition(m_ptPlayer);
	m_Camera.SetAnchor(Point2F(0.0f, 0.0f));
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
	//
	//auto wicFactory = m_pIndRes->wicFactory();
	//LoadImageFromFile(wicFactory
	//	, rendertarget.Get()
	//	, L"Hit1.png"
	//	, &m_pd2dbmpTest
	//);

	return true;
}

void CTestScene::Update(float fTimeElapsed)
{
	//fPositionX += (100 * fTimeElapsed);
	//if (fPositionX > 800) fPositionX -= 800.f;
	//
	//if ((currImg += fTimeElapsed * 10) >= 4.f)
	//	currImg -= 4.f;
	m_Camera.SetPosition(m_ptPlayer);
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

	pd2dRenderTarget->FillRectangle(
		RectF(-10, -10, 10, 10) + m_ptPlayer
		, m_pd2dsbrDefault.Get());
	pd2dRenderTarget->DrawRectangle(
		RectF(60, 60, 80, 80)
		, m_pd2dsbrDefault.Get());

	//pd2dRenderTarget->DrawRectangle(
	//	RectF(-50 + fPositionX, 100, 50 + fPositionX, 200)
	//	, m_pd2dsbrDefault.Get());
	//
	//pd2dRenderTarget->DrawTextLayout(
	//	  Point2F(0, 0)
	//	, m_pdwTextLayout.Get()
	//	, m_pd2dsbrDefault.Get()
	//);
	//
	//pd2dRenderTarget->DrawText(
	//	  L"text"
	//	, 4
	//	, m_pdwTextFormat.Get()
	//	, RectF(400, 500, 600, 600)
	//	, m_pd2dsbrDefault.Get()
	//);
	//
	//auto bmpSize = m_pd2dbmpTest->GetSize();
	//
	//pd2dRenderTarget->DrawBitmap(
	//	  m_pd2dbmpTest.Get()
	//	, RectF(100, 100, 100 + bmpSize.width * 0.25f, 100 + bmpSize.height)
	//	, min(1.f, currImg * 0.5f)
	//	, D2D1_BITMAP_INTERPOLATION_MODE_LINEAR
	//	, RectF(  bmpSize.width * (int)currImg * 0.25f, 0
	//			, bmpSize.width * ((int)currImg + 1) * 0.25f, bmpSize.height)
	//);

}
