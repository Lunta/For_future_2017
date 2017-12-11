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

	unique_ptr<CItem> PickItem();
	bool ProcessingMouseInput(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);

private:

	ComPtr<ID2D1SolidColorBrush>		m_hbrCaption;
	ComPtr<ID2D1SolidColorBrush>		m_hbrClient;
	ComPtr<ID2D1SolidColorBrush>		m_hbrText;

	ComPtr<IDWriteTextFormat>			m_dwTextFormat;

	ComPtr<ID2D1Bitmap1>				m_bmpFrame;

	D2D_SIZE_F							m_szInventoryOutlineMargin;

	D2D_SIZE_F							m_szItemBetweenMargin;
	D2D_SIZE_U							m_szItemMatrix;
	D2D_RECT_F							m_rcItem;

	D2D_SIZE_F							m_szEquipmentBetweenMargin;
	D2D_SIZE_U							m_szEquipmentMatrix;
	D2D_RECT_F							m_rcEquipment;

	list<unique_ptr<CItem>>				m_lstItem;
	vector<unique_ptr<CItem>>			m_vecEquipment;

	bool								m_bMouseDrag;

	D2D_POINT_2F						m_ptMousePos;
	D2D_POINT_2F						m_ptMouseDragPos;
};
