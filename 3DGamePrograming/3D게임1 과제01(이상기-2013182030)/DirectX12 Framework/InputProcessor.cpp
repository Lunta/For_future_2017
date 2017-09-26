#include "stdafx.h"

#include "InputProcessor.h"

CInputProcessor::CInputProcessor() :
	m_hWnd(NULL),
	m_pKeyBuffer(),
	m_dwDirection(0),
	m_bPushShift(false),
	m_bPushControll(false),
	m_bInput(false),
	m_ptOldCursorPos(),
	m_fResponsiveness(20.0f)
{
}

CInputProcessor::CInputProcessor(HWND hwnd, tagBeingIn tag) :
	m_hWnd(hwnd),
	m_tag(tag),
	m_pKeyBuffer(),
	m_dwDirection(0),
	m_bPushShift(false),
	m_bPushControll(false),
	m_bInput(false),
	m_ptOldCursorPos(),
	m_fResponsiveness(20.0f)
{
}

CInputProcessor::~CInputProcessor()
{
}

void CInputProcessor::SetMouseCapture(HWND hwnd)
{
	SetCapture(hwnd);
	GetCursorPos(&m_ptOldCursorPos);
}
void CInputProcessor::ReleaseMouseCapture()
{
	ReleaseCapture();
}

void CInputProcessor::SetMouseRsponsiveness(float fResponsiveness)
{
	m_fResponsiveness = fResponsiveness;
}
