#pragma once

class CFramework;
class CSoundManager;

class CScene
{

public:

	enum class CurrentScene : int {
		None = 0
		, Title
		, Lobby
		, Count
	};

protected:

	CFramework		*m_pFramework{ nullptr };
	CSoundManager	*m_pSound{ nullptr };

	HWND			m_hWnd;
	RECT			m_rcScene{ 0, 0, 0, 0 };

	CurrentScene	m_Tag;

public:
	CScene();
	virtual ~CScene();
	
	// 함수 호출 이후에도 처리를 원하면 false 반환
	virtual bool OnProcessingKeyboardMessage	(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	virtual bool OnProcessingMouseMessage		(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	virtual bool OnProcessingWindowMessage		(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);

	virtual void BuildObjects(CFramework* pFramework, CSoundManager *pSoundManager, HWND hwnd, CurrentScene tag);
	virtual void ReleaseObjects() = 0;

	virtual void Update(float fTimeElapsed) = 0;
	virtual void Render() = 0;

	CFramework* GetMyFramework() { return m_pFramework; }

	bool IsMyTag(CurrentScene tag) const { return (tag == m_Tag); }
	CurrentScene GetMyTag() const { return m_Tag; };
};

