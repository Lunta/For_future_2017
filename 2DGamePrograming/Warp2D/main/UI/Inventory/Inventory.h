#pragma once

#include "UI/UIBase.h"

class CItem;

class CUIInventory
	: public CUIBase
{
public:
	CUIInventory();
	~CUIInventory();

	void DrawClient(ID2D1HwndRenderTarget* pd2dRenderTarget) override;
	void DrawCaption(ID2D1HwndRenderTarget* pd2dRenderTarget) override;
	
	void BuildObject(CScene* scene) override;
	
	void Update(float fTimeElapsed) override;
	void GetItem(unique_ptr<CItem>&& item) { m_lstItem.push_back(std::move(item)); }
	void PutItem() { if (m_lstItem.size() > 0) m_lstItem.pop_back(); }
private:

	ComPtr<ID2D1SolidColorBrush>		m_hbrCaption;
	ComPtr<ID2D1SolidColorBrush>		m_hbrClient;
	ComPtr<ID2D1SolidColorBrush>		m_hbrText;

	ComPtr<ID2D1Bitmap1>				m_bmpFrame;

	D2D_SIZE_F							m_szItemOutlineMargin;
	D2D_SIZE_F							m_szItemBetweenMargin;
	D2D_SIZE_U							m_szItemMatrix;

	D2D_RECT_F							m_rcItem;

	list<unique_ptr<CItem>>				m_lstItem;


	ComPtr<IDWriteTextFormat>			m_dwTextFormat;
};
