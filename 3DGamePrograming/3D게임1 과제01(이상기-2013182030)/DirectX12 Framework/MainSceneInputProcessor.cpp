#include "stdafx.h"

#include "MainSceneInputProcessor.h"
#include "Player.h"

CMainSceneInputProcessor::CMainSceneInputProcessor(HWND hwnd, tagBeingIn tag, CPlayer* player) :m_pPlayer(player)
{
	m_hWnd = hwnd;
	m_tag = tag;
}

CMainSceneInputProcessor::~CMainSceneInputProcessor()
{
}

bool CMainSceneInputProcessor::ProcessInput(float fTimeElapsed, bool bProcessedByScene)
{
	if (!bProcessedByScene)
	{
		m_bInput = false;
		m_bPushShift = false;
		m_bPushControll = false;

		m_dwDirection = 0;

		if (GetKeyboardState(m_pKeyBuffer))
		{
			if (m_pKeyBuffer[VK_LSHIFT	] & 0xF0) m_bPushShift = true;
			if (m_pKeyBuffer[VK_CONTROL	] & 0xF0) m_bPushControll = true;
			if (m_pKeyBuffer['W'] & 0xF0) m_dwDirection |= DIR_FORWARD;
			if (m_pKeyBuffer['S'] & 0xF0) m_dwDirection |= DIR_BACKWARD;
			if (m_pKeyBuffer['A'] & 0xF0) m_dwDirection |= DIR_LEFT;
			if (m_pKeyBuffer['D'] & 0xF0) m_dwDirection |= DIR_RIGHT;
			if (m_pKeyBuffer['R'] & 0xF0) m_dwDirection |= DIR_UP;
			if (m_pKeyBuffer['F'] & 0xF0) m_dwDirection |= DIR_DOWN;

			if (m_pKeyBuffer[VK_SPACE] & 0xF0)
			{
				m_bInput = true;
				m_pPlayer->Input(CPlayer::InputKey::Shoot);
			}
		}

		float cxDelta = 0.0f, cyDelta = 0.0f;
		POINT ptCursorPos;
		if (GetCapture() == m_hWnd)
		{
			SetCursor(NULL);
			GetCursorPos(&ptCursorPos);
			cxDelta = static_cast<float>(ptCursorPos.x - m_ptOldCursorPos.x);
			cyDelta = static_cast<float>(ptCursorPos.y - m_ptOldCursorPos.y);
			SetCursorPos(m_ptOldCursorPos.x, m_ptOldCursorPos.y);
		}
		if ((m_dwDirection != 0) || (cxDelta != 0.0f) || (cyDelta != 0.0f))
		{
			if (cxDelta || cyDelta)
			{
				if (m_pKeyBuffer[VK_RBUTTON] & 0xF0)
				{
					m_pPlayer->Rotate(
						cyDelta * m_fResponsiveness * fTimeElapsed, 
						0.0f, -cxDelta * m_fResponsiveness * fTimeElapsed);
				}
				else
				{
					m_pPlayer->Rotate(cyDelta * m_fResponsiveness * fTimeElapsed, 
						cxDelta * m_fResponsiveness * fTimeElapsed, 0.0f);
				}
			}
			if (m_dwDirection)
			{
				m_bInput = true;
				m_pPlayer->Move(m_dwDirection, fTimeElapsed);
			}
		}

		if (!m_bInput)
			m_pPlayer->Input(CPlayer::InputKey::KeyUp);

		return m_bInput;
	}
	return false;
}
