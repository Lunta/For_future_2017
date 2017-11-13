#include "stdafx.h"
#include "Framework\Warp2DFramework.h"
#include "Framework\IndRes\IndRes.h"
#include "Scene\Scene.h"
#include "Object\Item\Item.h"
#include "Inventory.h"

CUIInventory::CUIInventory()
	: m_pItemList(nullptr)
{
}

CUIInventory::~CUIInventory()
{
}

void CUIInventory::DrawClient(ID2D1HwndRenderTarget * pd2dRenderTarget)
{
	pd2dRenderTarget->FillRectangle(m_rcClient, m_hbrClient.Get());
	pd2dRenderTarget->DrawBitmap(m_bmpFrame.Get(), m_rcItem);
	if (m_pItemList)
		for(auto& p: *m_pItemList)
		pd2dRenderTarget->DrawBitmap(
			p->GetBitmap(), m_rcItem);
}

void CUIInventory::DrawCaption(ID2D1HwndRenderTarget * pd2dRenderTarget)
{
	pd2dRenderTarget->FillRectangle(m_rcCaption, m_hbrCaption.Get());
	pd2dRenderTarget->DrawText(
		m_strTitle.c_str()
		, static_cast<UINT>(m_strTitle.length())
		, m_pdwTextFormat.Get()
		, m_rcCaption
		, m_hbrText.Get());
}

void CUIInventory::BuildObject(CScene * scene)
{
	auto framework = scene->GetFramework();
	auto indres = framework->GetIndRes();
	auto rendertarget = framework->GetRenderTarget();

	m_rcCaption = RectF(0, 0, 200, 20);
	m_rcClient = RectF(0, 0, 200, 220);

	m_ptOrigin = Point2F(560, 20);
	m_rcItem = RectF(10, 10, 50, 50);

	LoadImageFromFile(
		  indres->wicFactory()
		, rendertarget.Get()
		, L"Assets/Icon/Icon Frame.png"
		, &m_bmpFrame);

	rendertarget->CreateSolidColorBrush(ColorF{ ColorF::Yellow }, &m_hbrCaption);
	rendertarget->CreateSolidColorBrush(ColorF{ ColorF::White }, &m_hbrClient);
	rendertarget->CreateSolidColorBrush(ColorF{ ColorF::Black }, &m_hbrText);
	
	indres->dwFactory()->CreateTextFormat(
		L"Arial"
		, nullptr
		, DWRITE_FONT_WEIGHT_NORMAL
		, DWRITE_FONT_STYLE_NORMAL
		, DWRITE_FONT_STRETCH_NORMAL
		, (m_rcCaption.bottom - m_rcCaption.top) * 0.8f
		, L"ko-kr"
		, &m_pdwTextFormat);

	m_strTitle = L"Inventory"s;
}

void CUIInventory::Update(float fTimeElapsed)
{
}
