#pragma once
#include "Object\Unit\Unit.h"

class CIndRes;
class CItem;
class CPlayer : public CUnit
{
public:
	CPlayer(D2D_POINT_2F pt = Point2F(), D2D_RECT_F rc = RectF());
	virtual ~CPlayer();
	virtual void Update(float fTimeElapsed) override;
	virtual void Draw(ID2D1HwndRenderTarget* pd2dRenderTarget) override;

	virtual void RegisterSpriteImage(
		  CIndRes*					pIndRes
		, ID2D1HwndRenderTarget*	pd2dRenderTarget
		, path						filename
		, D2D_POINT_2F				ptLength);

	void Move(const D2D_POINT_2F& pt);
	void Move(float x, float y);

	bool PickUpItem(CItem& item);
	list<CItem>* GetpItemList() { return &m_ItemList; }

protected:
	D2D_POINT_2F		m_ptCurrImg;
	UINT				m_nSpriteImgWidth;
	UINT				m_nSpriteImgHeight;

	UINT				m_nMaxItems;
	list<CItem>			m_ItemList;
};

