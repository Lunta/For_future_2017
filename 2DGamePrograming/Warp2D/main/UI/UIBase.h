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

	void View(bool bView) { m_bViewUI = bView; }
	void SwitchView() { m_bViewUI = !m_bViewUI; }

protected:

	bool			m_bViewUI			{ true }	;
	bool			m_bViewCaption		{ true }	;
	bool			m_bViewClient		{ true }	;

	D2D_POINT_2F	m_ptOrigin;
	D2D_RECT_F		m_rcClient;
	D2D_RECT_F		m_rcCaption;

	wstring			m_strTitle;

};