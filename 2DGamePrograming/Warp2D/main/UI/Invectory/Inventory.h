#pragma once
#include "UI\UIBase.h"

#define INVENTORY_WIDTH 200
#define INVENTORY_CAPTION_HEIGHT 20
#define INVENTORY_CLIENT_HEIGHT 200
#define INVENTORY_DEFAULT_POS_WIDTH_RATIO 0.7
#define INVENTORY_DEFAULT_POS_HEIGHT_RATIO 0.04
#define INVENTORY_FRAME_SIZE 40

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

	void ConnectItemList(list<CItem>* item_list) { m_pItemList = item_list; }
private:
	ComPtr<ID2D1SolidColorBrush>		m_hbrCaption;
	ComPtr<ID2D1SolidColorBrush>		m_hbrClient;
	ComPtr<ID2D1SolidColorBrush>		m_hbrText;

	ComPtr<ID2D1Bitmap1>				m_bmpFrame;
	UINT								m_nItemFrames;
	D2D_RECT_F*							m_prcItemFrames;

	list<CItem>*						m_pItemList;

	ComPtr<IDWriteTextFormat>			m_pdwTextFormat;
};