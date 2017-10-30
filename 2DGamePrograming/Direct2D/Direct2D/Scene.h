#pragma once

class CFramework;

class CScene
{
public:
	enum class CurrentScene : int {
		None = 0
		, Main
		, count
	};

protected:
	CFramework		*m_pFramework;

	HWND			m_hWnd;
	RECT			m_rcScene;

	CurrentScene	m_Tag;

public:
	CScene();
	~CScene();
	
	// 함수 호출 이후에도 처리를 원하면 false 반환
	virtual bool OnProcessingKeyboardMessage	(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	virtual bool OnProcessingMouseMessage		(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	virtual bool OnProcessingWindowMessage		(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);

	virtual void BuildObjects(CFramework* pFramework, HWND hwnd, CurrentScene tag);
	virtual void ReleaseObjects() = 0;

	virtual void Update(float fTimeElapsed) = 0;
	virtual void Render(ID2D1HwndRenderTarget* pRendertarget) = 0;

	CFramework* GetMyFramework() { return m_pFramework; }

	bool IsMyTag(CurrentScene tag) const { return (tag == m_Tag); }
	CurrentScene GetMyTag() const { return m_Tag; };
};

