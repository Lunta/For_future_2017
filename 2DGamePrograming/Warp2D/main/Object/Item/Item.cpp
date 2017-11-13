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
	m_angle += fTimeElapsed * 100;
}
void CItem::Draw(ID2D1HwndRenderTarget * pd2dRenderTarget)
{
	D2D_MATRIX_3X2_F transform;
	pd2dRenderTarget->GetTransform(&transform);
	auto tr = Matrix3x2F::Rotation(m_angle, m_ptPos)*transform;
	pd2dRenderTarget->SetTransform(tr);
	pd2dRenderTarget->DrawBitmap(
		m_bmpImage.Get()
		, m_rcSize + m_ptPos);
	pd2dRenderTarget->SetTransform(transform);
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
	if (IsRectInvaild(m_rcSize))
		m_rcSize = SizeToRect(m_bmpImage->GetSize());
}

void CItem::RegisterImage(const ComPtr<ID2D1Bitmap1>& bmp)
{
	m_bmpImage = bmp;
	if (IsRectInvaild(m_rcSize))
		m_rcSize = SizeToRect(m_bmpImage->GetSize());
}

void CItem::RegisterImage(ComPtr<ID2D1Bitmap1>&& bmp) noexcept
{
	m_bmpImage = move(bmp);
	if (IsRectInvaild(m_rcSize))
		m_rcSize = SizeToRect(m_bmpImage->GetSize());
}