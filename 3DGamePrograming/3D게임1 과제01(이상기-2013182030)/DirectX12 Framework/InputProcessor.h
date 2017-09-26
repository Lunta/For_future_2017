#pragma once

class CInputProcessor
{
public:
	enum class tagBeingIn : int {
		MainScene = 0
		, Count
	};
protected:
	HWND		m_hWnd;

	tagBeingIn	m_tag;

	UCHAR		m_pKeyBuffer[KEY_BUFFER_SIZE];
	DWORD		m_dwDirection;
				
	bool		m_bPushShift;
	bool		m_bPushControll;
	bool		m_bInput;

	POINT		m_ptOldCursorPos;

	float		m_fResponsiveness;

public:
	CInputProcessor();
	CInputProcessor(HWND hwnd, tagBeingIn tag);
	virtual ~CInputProcessor();

	virtual bool ProcessInput(float fTimeElapsed, bool bProcessedByScene = false) = 0;

	void SetMouseCapture(HWND hwnd);
	void ReleaseMouseCapture();

	void SetMouseRsponsiveness(float fResponsiveness);
};

