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
	float fItemWidth = m_rcItem.right - m_rcItem.left;
	float fItemHeight = m_rcItem.bottom - m_rcItem.top;
	int iItemMatrixHeight = static_cast<int>(m_szItemMatrix.height);
	int iItemMatrixWidth = static_cast<int>(m_szItemMatrix.width);

	for (int iColumn = 0; iColumn < iItemMatrixHeight; ++iColumn)
		for (int iRow = 0; iRow < iItemMatrixWidth; ++iRow)
		{
			float fWidth = 
				  fItemWidth * iRow
				+ m_szItemBetweenMargin.width * max(0, iRow) 
				+ m_szInventoryOutlineMargin.width;
			float fHeight = 
				  fItemHeight * iColumn
				+ m_szItemBetweenMargin.height * max(0, iColumn) 
				+ m_szInventoryOutlineMargin.height;

			auto rc = RectF(
				  fWidth + m_rcItem.left
				, fHeight + m_rcItem.top
				, fWidth + m_rcItem.right
				, fHeight + m_rcItem.bottom);

			pd2dRenderTarget->DrawBitmap(m_bmpFrame.Get(), rc);

			if (pItem != end(m_lstItem))
			{
				pd2dRenderTarget->DrawBitmap((*pItem)->GetBitmap(), rc);
				++pItem;
			}

		}

	auto pEquipment = begin(m_vecEquipment);
	float fEquipmentWidth = m_rcEquipment.right - m_rcEquipment.left;
	float fEquipmentHeight = m_rcEquipment.bottom - m_rcEquipment.top;
	int iEquipmentMatrixHeight = static_cast<int>(m_szEquipmentMatrix.height);
	int iEquipmentMatrixWidth = static_cast<int>(m_szEquipmentMatrix.width);

	for (int iColumn = 0; iColumn < iEquipmentMatrixHeight; ++iColumn)
		for (int iRow = 0; iRow < iEquipmentMatrixWidth; ++iRow)
		{
			float fWidth = 
				fEquipmentWidth * iRow
				+ m_szEquipmentBetweenMargin.width * (max(0, iRow) + 1)
				+ m_szInventoryOutlineMargin.width;
			float fHeight =
				fItemHeight * iItemMatrixHeight
				+ m_szItemBetweenMargin.height * iItemMatrixHeight
				+ fEquipmentHeight * iColumn 
				+ m_szEquipmentBetweenMargin.height * (max(0, iColumn) + 1)
				+ m_szInventoryOutlineMargin.height;

			auto rc = RectF(
				fWidth + m_rcEquipment.left
				, fHeight + m_rcEquipment.top
				, fWidth + m_rcEquipment.right
				, fHeight + m_rcEquipment.bottom);

			pd2dRenderTarget->DrawBitmap(m_bmpFrame.Get(), rc);

			if (pEquipment != end(m_vecEquipment))
			{
				pd2dRenderTarget->DrawBitmap((*pEquipment)->GetBitmap(), rc);
				++pEquipment;
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
	m_rcClient = RectF(0, 0, 200, 320);

	m_ptOrigin = Point2F(560, 20);

	m_szInventoryOutlineMargin = SizeF(10, 20);

	m_rcItem = RectF(0, 0, 40, 40);
	m_szItemMatrix = SizeU(4, 5);

	m_rcEquipment = RectF(0, 0, 50, 50);
	m_szEquipmentMatrix = SizeU(3, 1);
	m_vecEquipment.reserve(m_szEquipmentMatrix.width * m_szEquipmentMatrix.height);

	float fClientWidth = m_rcClient.right - m_rcClient.left;
	float fClientHeight = m_rcClient.bottom - m_rcClient.top;

	float fItemWidths = (m_rcItem.right - m_rcItem.left) * m_szItemMatrix.width;
	float fItemHeights = (m_rcItem.bottom - m_rcItem.top) * m_szItemMatrix.height;

	m_szItemBetweenMargin.width = (
		(m_szItemMatrix.width == 1) ? 0 
		: (fClientWidth - (fItemWidths + 2.f * m_szInventoryOutlineMargin.width)) 
		  / static_cast<float>(m_szItemMatrix.width - 1));

	m_szItemBetweenMargin.height = m_szItemBetweenMargin.width;

	float fEquipmentWidths = (m_rcEquipment.right - m_rcEquipment.left) * m_szEquipmentMatrix.width;
	float fEquipmentHeights = (m_rcEquipment.bottom - m_rcEquipment.top) * m_szEquipmentMatrix.height;

	m_szEquipmentBetweenMargin.width = 
		(fClientWidth - (fEquipmentWidths + 2.f * m_szInventoryOutlineMargin.width))
		  / static_cast<float>(m_szEquipmentMatrix.width + 1);

	m_szEquipmentBetweenMargin.height = 
		(fClientHeight
			- (fItemHeights + fEquipmentHeights 
				+ m_szItemBetweenMargin.height * (m_szItemMatrix.height - 1)
				+ 2.f * m_szInventoryOutlineMargin.height))
		  / static_cast<float>(m_szEquipmentMatrix.height + 1);

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

bool CUIInventory::ProcessingMouseInput(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	switch (nMessageID)
	{
	case WM_LBUTTONDOWN:	
		m_bMouseDrag = true;
		m_ptMousePos.x = LOWORD(lParam); 
		m_ptMousePos.y = HIWORD(lParam);
		break;
	case WM_LBUTTONUP:		
		m_bMouseDrag = false;
		m_ptMousePos = Point2F();
		m_ptMouseDragPos = Point2F();
		break;
	case WM_RBUTTONDOWN:	
		m_ptMousePos.x = LOWORD(lParam);
		m_ptMousePos.y = HIWORD(lParam);
		break;
	case WM_RBUTTONUP:		
		m_ptMousePos = Point2F();
		break;
	case WM_MOUSEMOVE:
		if (m_bMouseDrag)
		{
			m_ptMouseDragPos.x = LOWORD(lParam);
			m_ptMouseDragPos.y = HIWORD(lParam);
		}
		break;
	case WM_MBUTTONDOWN: break;
	case WM_MBUTTONUP: break;
	case WM_MOUSEWHEEL: break;
	default:
		return false;
	}

	return(true);
}
