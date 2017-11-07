#include "stdafx.h"
#include "Framework\IndRes\IndRes.h"
#include "Item.h"


CItem::CItem(D2D_POINT_2F pt, D2D_RECT_F rc)
	:CObject(pt, rc)
{
}
CItem::~CItem()
{
}

void CItem::Update(float fTimeElapsed)
{
}
void CItem::Draw(ID2D1HwndRenderTarget * pd2dRenderTarget)
{
	pd2dRenderTarget->DrawBitmap(
		  m_bmpImage.Get()
		, m_rcSize + m_ptPos);
}

void CItem::RegisterImage(
	  CIndRes*					pIndRes
	, ID2D1HwndRenderTarget*	pd2dRenderTarget
	, path						filename)
{
	LoadImageFromFile(
		  pIndRes->wicFactory()
		, pd2dRenderTarget
		, filename.c_str()
		, &m_bmpImage
	);
	if (	m_rcSize.left == 0.f
		&&	m_rcSize.right == 0.f
		&&	m_rcSize.top == 0.f
		&&	m_rcSize.bottom == 0.f)
	{
		auto sz = m_bmpImage->GetSize();
		m_rcSize = RectF(
			  -sz.width * 0.5f
			, -sz.height * 0.5f
			, sz.width * 0.5f
			, sz.height * 0.5f);
	}
}

void CItem::RegisterImage(const ComPtr<ID2D1Bitmap1>& bmp)
{
	m_bmpImage = bmp;
	if (m_rcSize.left == 0.f
		&&	m_rcSize.right == 0.f
		&&	m_rcSize.top == 0.f
		&&	m_rcSize.bottom == 0.f)
	{
		auto sz = m_bmpImage->GetSize();
		m_rcSize = RectF(
			-sz.width * 0.5f
			, -sz.height * 0.5f
			, sz.width * 0.5f
			, sz.height * 0.5f);
	}
}

void CItem::RegisterImage(ComPtr<ID2D1Bitmap1>&& bmp) noexcept
{
	m_bmpImage = move(bmp);
	if (m_rcSize.left == 0.f
		&&	m_rcSize.right == 0.f
		&&	m_rcSize.top == 0.f
		&&	m_rcSize.bottom == 0.f)
	{
		auto sz = m_bmpImage->GetSize();
		m_rcSize = RectF(
			-sz.width * 0.5f
			, -sz.height * 0.5f
			, sz.width * 0.5f
			, sz.height * 0.5f);
	}
}