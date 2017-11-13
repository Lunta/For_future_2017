#include "stdafx.h"
#include "Framework\IndRes\IndRes.h"
#include "Unit.h"

CUnit::CUnit(D2D_POINT_2F pt, D2D_RECT_F rc)
	: CObject(pt, rc)
{
}
CUnit::~CUnit()
{
}

void CUnit::Update(float fTimeElapsed)
{
}

void CUnit::Draw(ID2D1HwndRenderTarget * pd2dRenderTarget)
{
	pd2dRenderTarget->DrawBitmap(
		m_bmpImage.Get()
		, m_rcSize + m_ptPos);
}

void CUnit::RegisterImage(CIndRes * pIndRes, ID2D1HwndRenderTarget * pd2dRenderTarget, path filename)
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

void CUnit::RegisterImage(const ComPtr<ID2D1Bitmap1>& bmp)
{
	m_bmpImage = bmp;
	if (IsRectInvaild(m_rcSize))
		m_rcSize = SizeToRect(m_bmpImage->GetSize());
}

void CUnit::RegisterImage(ComPtr<ID2D1Bitmap1>&& bmp) noexcept
{
	m_bmpImage = move(bmp);
	if (IsRectInvaild(m_rcSize))
		m_rcSize = SizeToRect(m_bmpImage->GetSize());
}
