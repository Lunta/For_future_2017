#pragma once

class CWarp2DFramework;
class CIndRes;

class CScene
{
public:
	CScene();
	virtual ~CScene();

	virtual bool OnCreate(wstring&& tag, CWarp2DFramework* pFramework);

	virtual bool OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	virtual bool OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	virtual bool OnProcessingWindowMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);

	virtual void Update(float fTimeElapsed) = 0;
	virtual void Draw(ID2D1HwndRenderTarget * pd2dRenderTarget) = 0;

	bool FindByTag(const wstring& Tag) const { return Tag == m_strTag; }

	const wstring& Tag() const { return m_strTag; }

protected:

	CWarp2DFramework*		m_pFramework	{ nullptr }	;
	HWND					m_hWnd			{ NULL }	;
	shared_ptr<CIndRes>		m_pIndRes		{ nullptr }	;

	wstring					m_strTag					;

};
