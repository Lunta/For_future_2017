#pragma once

class CScene;

class CUIBase
{
public:
	CUIBase();
	virtual ~CUIBase();

	virtual void Draw(ID2D1HwndRenderTarget* pd2dRenderTarget);
	virtual void DrawClient(ID2D1HwndRenderTarget* pd2dRenderTarget);
	virtual void DrawCaption(ID2D1HwndRenderTarget* pd2dRenderTarget);

	virtual void BuildObject(CScene* scene);

	virtual void Update(float fTimeElapsed) {}

protected:
	bool			m_bViewUI;
	bool			m_bViewClient;
	bool			m_bViewCaption;

	D2D_POINT_2F	m_ptOrigin;
	D2D_RECT_F		m_rcClient;
	D2D_RECT_F		m_rcCaption;

	wstring			m_strTitle;

};