#include "stdafx.h"
#include "Framework\Warp2DFramework.h"
#include "Framework\IndRes\IndRes.h"
#include "Scene\Scene.h"
#include "Object\Item\Item.h"
#include "Inventory.h"

CUIInventory::CUIInventory()
	: m_prcItemFrames(nullptr)
	, m_pItemList(nullptr)
{
}

CUIInventory::~CUIInventory()
{
	if (m_prcItemFrames)
		delete[] m_prcItemFrames;
}

void CUIInventory::DrawClient(ID2D1HwndRenderTarget * pd2dRenderTarget)
{
	pd2dRenderTarget->FillRectangle(m_rcClient, m_hbrClient.Get());

	for (int i = 0; i < m_nItemFrames; ++i)
		pd2dRenderTarget->DrawBitmap(m_bmpFrame.Get(), m_prcItemFrames[i]);
	
	if (m_pItemList)
	{
		int i = 0;
		for(auto p: *m_pItemList)
			pd2dRenderTarget->DrawBitmap(
				p.GetBitmap()
				, m_prcItemFrames[i++]);
	}
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

	m_rcCaption = RectF(0, 0, INVENTORY_WIDTH
		, INVENTORY_CAPTION_HEIGHT);
	m_rcClient = RectF(0, 0, INVENTORY_WIDTH
		, INVENTORY_CAPTION_HEIGHT + INVENTORY_CLIENT_HEIGHT);
	
	m_ptOrigin = Point2F(
		CLIENT_WIDTH * INVENTORY_DEFAULT_POS_WIDTH_RATIO
		, CLIENT_HEIGHT * INVENTORY_DEFAULT_POS_HEIGHT_RATIO);

	int nFrameWidth = INVENTORY_WIDTH / INVENTORY_FRAME_SIZE;
	int nFrameHeight = INVENTORY_CLIENT_HEIGHT / INVENTORY_FRAME_SIZE;
	float stride_width = (INVENTORY_WIDTH - INVENTORY_FRAME_SIZE * (nFrameWidth - 1)) / (float)nFrameWidth;
	float stride_height = (INVENTORY_CLIENT_HEIGHT - INVENTORY_FRAME_SIZE * (nFrameHeight - 1)) / (float)nFrameHeight;
	D2D1_RECT_F rcFrame = SizeToRect(SizeF(
		  INVENTORY_FRAME_SIZE
		, INVENTORY_FRAME_SIZE));
	float hsize = (float)INVENTORY_FRAME_SIZE * 0.5f;

	m_nItemFrames = (--nFrameWidth)*(--nFrameHeight);
	m_prcItemFrames = new D2D_RECT_F[m_nItemFrames];

	int idx = 0;
	for(int i=0; i<nFrameHeight; ++i)
		for (int j = 0; j < nFrameWidth; ++j, ++idx)
		{
			m_prcItemFrames[idx] = rcFrame + Point2F(
				  stride_width + hsize + (stride_width + INVENTORY_FRAME_SIZE)*j
				, stride_height + hsize + (stride_height + INVENTORY_FRAME_SIZE) *i);
		}

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