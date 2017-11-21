#include "stdafx.h"
#include "Framework/Warp2DFramework.h"
#include "Framework/IndRes/IndRes.h"
#include "Scene/Scene.h"
#include "Object/Item/Item.h"
#include "Inventory.h"

CUIInventory::CUIInventory()
{
}

CUIInventory::~CUIInventory()
{
}

void CUIInventory::DrawClient(ID2D1HwndRenderTarget * pd2dRenderTarget)
{
	pd2dRenderTarget->FillRectangle(m_rcClient, m_hbrClient.Get());

	auto pItem = begin(m_lstItem);

	for (int iColumn = 0; iColumn < static_cast<int>(m_szItemMatrix.height); ++iColumn)
		for (int iRow = 0; iRow < static_cast<int>(m_szItemMatrix.width); ++iRow)
		{
			float fWidth = (m_rcItem.right - m_rcItem.left) * iRow + m_szItemBetweenMargin.width * max(0, iRow) + m_szItemOutlineMargin.width;
			float fHeight = (m_rcItem.bottom - m_rcItem.top) * iColumn + m_szItemBetweenMargin.height * max(0, iColumn) + m_szItemOutlineMargin.height;

			auto rc = RectF(fWidth + m_rcItem.left, fHeight + m_rcItem.top, fWidth + m_rcItem.right, fHeight + m_rcItem.bottom);

			pd2dRenderTarget->DrawBitmap(m_bmpFrame.Get(), rc);

			if (pItem != end(m_lstItem))
			{
				pd2dRenderTarget->DrawBitmap(
					(*(pItem++))->GetBitmap()
					, rc
				);
			}

		}
}

void CUIInventory::DrawCaption(ID2D1HwndRenderTarget * pd2dRenderTarget)
{
	pd2dRenderTarget->FillRectangle(m_rcCaption, m_hbrCaption.Get());
	pd2dRenderTarget->DrawText(
		  m_strTitle.c_str()
		, static_cast<UINT>(m_strTitle.length())
		, m_dwTextFormat.Get()
		, m_rcCaption
		, m_hbrText.Get());
}

void CUIInventory::BuildObject(CScene * scene)
{
	auto framework = scene->Framework();
	auto indres = framework->GetIndRes();
	auto rendertarget = framework->GetRenderTarget();

	m_rcCaption = RectF(0, 0, 200, 20);
	m_rcClient = RectF(0, 0, 200, 220);

	m_ptOrigin = Point2F(560, 20);

	m_rcItem = RectF(0, 0, 40, 40);

	m_szItemMatrix = SizeU(4, 4);
	m_szItemOutlineMargin = SizeF(10, 20);

	auto fClientWidth = m_rcClient.right - m_rcClient.left;
	auto fItemWidths = (m_rcItem.right - m_rcItem.left) * m_szItemMatrix.width;

	m_szItemBetweenMargin.width = (m_szItemMatrix.width == 1 ? 0 
		: (fClientWidth - (fItemWidths + 2.f * m_szItemOutlineMargin.width)) 
		/ static_cast<float>(m_szItemMatrix.width - 1)
	);

	m_szItemBetweenMargin.height = m_szItemBetweenMargin.width;

	LoadImageFromFile(
		  indres->wicFactory()
		, rendertarget.Get()
		, L"Graphics/Icon/Icon Frame.png"
		, &m_bmpFrame);

	rendertarget->CreateSolidColorBrush(ColorF{ ColorF::Yellow }, &m_hbrCaption);
	rendertarget->CreateSolidColorBrush(ColorF{ ColorF::DimGray }, &m_hbrClient);
	rendertarget->CreateSolidColorBrush(ColorF{ ColorF::Black }, &m_hbrText);
	
	indres->dwFactory()->CreateTextFormat(
		  L"Arial"
		, nullptr
		, DWRITE_FONT_WEIGHT_NORMAL
		, DWRITE_FONT_STYLE_NORMAL
		, DWRITE_FONT_STRETCH_NORMAL
		, (m_rcCaption.bottom - m_rcCaption.top) * 0.8f
		, L"ko-kr"
		, &m_dwTextFormat);

	m_strTitle = L"Inventory"s;
}

void CUIInventory::Update(float fTimeElapsed)
{

}
