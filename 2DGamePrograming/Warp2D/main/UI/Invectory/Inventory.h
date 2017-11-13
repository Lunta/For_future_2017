#pragma once
#include "UI\UIBase.h"

class CItem;
class CUIInventory : public CUIBase
{
public:
	CUIInventory();
	virtual ~CUIInventory();

	virtual void DrawClient(ID2D1HwndRenderTarget* pd2dRenderTarget) override;
	virtual void DrawCaption(ID2D1HwndRenderTarget* pd2dRenderTarget) override;

	virtual void BuildObject(CScene* scene) override;

	virtual void Update(float fTimeElapsed) override;

	void SetPlayer() {  }
private:
	ComPtr<ID2D1SolidColorBrush>		m_hbrCaption;
	ComPtr<ID2D1SolidColorBrush>		m_hbrClient;
	ComPtr<ID2D1SolidColorBrush>		m_hbrText;

	ComPtr<ID2D1Bitmap1>				m_bmpFrame;
	D2D_RECT_F							m_rcItem;

	list<CItem*>*						m_pItemList;

	ComPtr<IDWriteTextFormat>			m_pdwTextFormat;
};