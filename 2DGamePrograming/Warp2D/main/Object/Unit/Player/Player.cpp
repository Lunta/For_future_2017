#include "stdafx.h"
#include "Framework\IndRes\IndRes.h"
#include "Object\Item\Item.h"
#include "Player.h"

CPlayer::CPlayer(D2D_POINT_2F pt, D2D_RECT_F rc)
	: CUnit(pt, rc)
	, m_nSpriteImgWidth(0)
	, m_nSpriteImgHeight(0)
	, m_nMaxItems(5)
{
}

CPlayer::~CPlayer()
{
}

void CPlayer::Update(float fTimeElapsed)
{
	if ((m_ptCurrImg.x += fTimeElapsed*10) >= m_nSpriteImgWidth)
		m_ptCurrImg.x -= static_cast<float>(m_nSpriteImgWidth);
}

void CPlayer::Draw(ID2D1HwndRenderTarget * pd2dRenderTarget)
{
	auto bmpSize = m_bmpImage->GetSize();

	float aniWidthFactor = bmpSize.width / (float)m_nSpriteImgWidth;
	float aniHeightFactor = bmpSize.height / (float)m_nSpriteImgHeight;
	pd2dRenderTarget->DrawBitmap(
		m_bmpImage.Get()
		, m_rcSize + m_ptPos
		, 1.f
		, D2D1_BITMAP_INTERPOLATION_MODE_LINEAR
		, RectF(  
			  aniWidthFactor * (int)m_ptCurrImg.x 
			, aniHeightFactor * (int)m_ptCurrImg.y
			, aniWidthFactor * ((int)m_ptCurrImg.x + 1) 
			, aniHeightFactor * ((int)m_ptCurrImg.y + 1))
	);
}

void CPlayer::RegisterSpriteImage(
	  CIndRes*					pIndRes
	, ID2D1HwndRenderTarget*	pd2dRenderTarget
	, path						filename
	, D2D_POINT_2F				ptLength)
{
	m_nSpriteImgWidth = ptLength.x;
	m_nSpriteImgHeight = ptLength.y;

	LoadImageFromFile(
		pIndRes->wicFactory()
		, pd2dRenderTarget
		, filename.c_str()
		, &m_bmpImage
	);
	if (IsRectInvaild(m_rcSize))
	{
		auto sz = m_bmpImage->GetSize();
		sz.width /= ptLength.x;
		sz.height /= ptLength.y;
		m_rcSize = SizeToRect(sz);
	}
}

void CPlayer::Move(const D2D_POINT_2F & pt)
{
	m_ptPos = m_ptPos + pt;
	if (pt.x > 0.f)
		m_ptCurrImg.y = 2;
	else if (pt.x < 0.f)
		m_ptCurrImg.y = 1;
	if (pt.y > 0.f)
		m_ptCurrImg.y = 0;
	else if (pt.y < 0.f)
		m_ptCurrImg.y = 3;
}
void CPlayer::Move(float x, float y)
{
	m_ptPos = m_ptPos + Point2F(x,y);
	if (x > 0.f)
		m_ptCurrImg.y = 2;
	else if (x < 0.f)
		m_ptCurrImg.y = 1;
	if (y > 0.f)
		m_ptCurrImg.y = 0;
	else if (y < 0.f)
		m_ptCurrImg.y = 3;
}

bool CPlayer::PickUpItem(CItem& item)
{
	if (m_ItemList.size() < m_nMaxItems)
	{
		m_ItemList.emplace_back(item);
		return true;
	}
	return false;
}
