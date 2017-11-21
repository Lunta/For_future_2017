#include "stdafx.h"

#include "Framework/Warp2DFramework.h"
#include "Framework/IndRes/IndRes.h"

#include "TestScene.h"


CTestScene::CTestScene()
	: m_Player { SizeU(25, 25) }
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
		}
		break;

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
		case 'A':		m_Player.Move(CPlayer::Dir::left);
			break;
		case 'W':		m_Player.Move(CPlayer::Dir::top);
			break;
		case 'D':		m_Player.Move(CPlayer::Dir::right);
			break;
		case 'S':		m_Player.Move(CPlayer::Dir::bottom);
			break;
		case 'Z':		m_Camera.Scale(m_Camera.GetScale() * 2.f);
			break;
		case 'X':		m_Camera.Scale(m_Camera.GetScale() * 0.5f);
			break;
		case 'H':		m_uiInventory.PutItem();
			break;
		case 'I':		m_uiInventory.SwitchView();
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

	auto rcClient = m_pFramework->GetClientSize();
	m_Camera.SetClientSize(Point2F(rcClient.right, rcClient.bottom));

	auto rendertarget = m_pFramework->GetRenderTarget();

	rendertarget->CreateSolidColorBrush(ColorF{ ColorF::Crimson }, &m_pd2dsbrDefault);
	rendertarget->CreateSolidColorBrush(ColorF{ ColorF::MistyRose }, &m_pd2dsbrTileA);
	rendertarget->CreateSolidColorBrush(ColorF{ ColorF::LightPink }, &m_pd2dsbrTileB);

	m_Camera.SetPosition(m_Player.GetPosition());
	m_Camera.SetAnchor(Point2F(0.f, 0.f));

	m_Player.RegisterImage(m_pIndRes.get(), rendertarget.Get(), "Graphics/player.png", SizeU(4,4));

	uniform_int_distribution<> pos_random{ 0,49 };
	uniform_int_distribution<> img_random{ 0,5 };
	default_random_engine reng(random_device{}());

	for (int i = 0; i < 10; ++i)
	{
		retryItem:
		auto item = make_unique<CItem>(SizeU(pos_random(reng), pos_random(reng)));
		for (const auto& p : m_lstItem) if (p->IsCollision(item->GetCoord())) goto retryItem;

		path imgPath;
		switch (img_random(reng))
		{
		case 0:	imgPath = "Graphics/Icon/Bastard Sword.png";
			break;
		case 1:	imgPath = "Graphics/Icon/Gramr.png";
			break;
		case 2:	imgPath = "Graphics/Icon/string.png";
			break;	
		case 3:	imgPath = "Graphics/Icon/Round Shield.png";
			break;
		case 4:	imgPath = "Graphics/Icon/Healing Potion.png";
			break;
		case 5:	imgPath = "Graphics/Icon/Mana Potion.png";
			break;

		}

		item->RegisterImage(m_pIndRes.get(), rendertarget.Get(), imgPath);
		m_lstItem.push_back(move(item));
	}

	for (int i = 0; i < 10; ++i)
	{
	retryTrap:
		auto item = make_unique<CItem>(SizeU(pos_random(reng), pos_random(reng)));
		for (const auto& p : m_lstItem) if (p->IsCollision(item->GetCoord())) goto retryTrap;
		for (const auto& p : m_lstTrap) if (p->IsCollision(item->GetCoord())) goto retryTrap;

		item->RegisterImage(m_pIndRes.get(), rendertarget.Get()
			, "Graphics/Icon/Wonder stone.png");
		m_lstTrap.push_back(move(item));
	}

	m_uiInventory.BuildObject(this);

	return true;
}

void CTestScene::Update(float fTimeElapsed)
{
	m_Camera.SetPosition(m_Player.GetPosition());
	m_uiInventory.Update(fTimeElapsed);

	m_Player.Update(fTimeElapsed);

	for (auto p = begin(m_lstItem); p != end(m_lstItem); ++p)
		if ((*p)->IsCollision(m_Player.GetCoord()))
		{
			m_uiInventory.GetItem(move(*p));
			m_lstItem.erase(p);
			break;
		}
	for (auto p = begin(m_lstTrap); p != end(m_lstTrap); ++p)
		if ((*p)->IsCollision(m_Player.GetCoord()))
		{
			m_Player.GetDamage(rand() % 10 + 10);
			m_lstTrap.erase(p);
			break;
		}
}

void CTestScene::Draw(ID2D1HwndRenderTarget * pd2dRenderTarget)
{
	auto cameramtx = m_Camera.RegenerateViewMatrix();
	pd2dRenderTarget->SetTransform(cameramtx);
	
	for (int x = 0; x < 50; ++x)
		for (int y = 0; y < 50; ++y)
			pd2dRenderTarget->FillRectangle(
				  GetPositionByCoord(SizeU(x,y)) + g_rcItemRect
				, (x + y) % 2 ? m_pd2dsbrTileA.Get() : m_pd2dsbrTileB.Get());

	m_Player.Draw(pd2dRenderTarget);

	for (const auto& p : m_lstItem)
		p->Draw(pd2dRenderTarget); 
	for (const auto& p : m_lstTrap)
		p->Draw(pd2dRenderTarget);

	m_Player.DrawUI(pd2dRenderTarget);
	m_uiInventory.Draw(pd2dRenderTarget);
}
