#pragma once
#include "InputProcessor.h"

class CPlayer;

class CMainSceneInputProcessor : public CInputProcessor
{
private:
	CPlayer		*m_pPlayer;

public:
	CMainSceneInputProcessor(HWND hwnd, tagBeingIn tag, CPlayer* player);
	~CMainSceneInputProcessor();

	virtual bool ProcessInput(float fTimeElapsed, bool bProcessedByScene = false) override;
};