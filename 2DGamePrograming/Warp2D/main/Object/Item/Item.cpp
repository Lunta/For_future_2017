#include "stdafx.h"
#include "Framework/IndRes/IndRes.h"
#include "Item.h"

CItem::CItem(D2D_SIZE_U sz)
	: CObject(Point2F(sz.width * g_fTileWidth, sz.height * g_fTileHeight), g_rcItemRect)
	, m_szCoord { sz }
{
}

CItem::~CItem()
{
}

void CItem::Update(float fTimeElapsed)
{

}

void CItem::Draw(ID2D1HwndRenderTarget * RenderTarget)
{
	RenderTarget->DrawBitmap(
		  m_bmpImage.Get()
		, m_rcSize + m_ptPoisition
	);
}

void CItem::RegisterImage(CIndRes * indres, ID2D1HwndRenderTarget * RenderTarget, path filename)
{
	LoadImageFromFile(
		  indres->wicFactory()
		, RenderTarget
		, filename.c_str()
		, &m_bmpImage
	);

}

void CItem::RegisterImage(const ComPtr<ID2D1Bitmap1>& bmp)
{
	m_bmpImage = bmp;
}

void CItem::RegisterImage(ComPtr<ID2D1Bitmap1>&& bmp) noexcept
{
	m_bmpImage = move(bmp);
}
