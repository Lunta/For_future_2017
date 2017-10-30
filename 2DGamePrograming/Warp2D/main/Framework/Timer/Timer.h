#pragma once

#include <chrono>

class CTimer
{
	using clock = std::chrono::system_clock;
public:
	CTimer();
	virtual ~CTimer();

	bool Update();
	float GetFPS() const { return { 0.5f + m_olddCumulativefps / static_cast<float>(m_oldnCumulativefpsCount) }; }
	float GetTimeElapsed() const { return m_TimeElapsed; }

	void SetUpdateCaptionHwnd(HWND hWnd);
	void UpdateCaption();

protected:

	clock::time_point m_current_time		{ clock::now() }	;

	float		m_TimeElapsed				{ 0.f }				;

	float		m_dCumulativefps			{ 0.f }				;
	int			m_nCumulativefpsCount		{ 0 }				;

	float		m_olddCumulativefps			{ 0.f }				;
	int			m_oldnCumulativefpsCount	{ 0 }				;

	clock::time_point m_LastUpdate_time		{ clock::now() }	;


	HWND		m_hWnd								{ nullptr }	;
	TCHAR		m_CaptionTitle[TITLE_MAX_LENGTH]				;
	int			m_TitleLength						{ 0 }		;

#if defined(SHOW_CAPTIONFPS)
	int			m_nLastFps							{ 0 }		;
#endif

};
