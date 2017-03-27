#pragma once
#include "Scene.h"

class CLobbyScene :public CScene
{
protected:


public:
	CLobbyScene();
	virtual ~CLobbyScene();

	virtual void Update(float fTimeElapsed); 
	virtual void Render();

	// 함수 호출 이후에도 처리를 원하면 false 반환
	virtual bool OnProcessingKeyboardMessage	(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	virtual bool OnProcessingMouseMessage		(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	virtual bool OnProcessingWindowMessage		(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);

	virtual void BuildObjects(CFramework* pFramework, CSoundManager *pSoundManager, HWND hwnd, CurrentScene tag);
	virtual void ReleaseObjects();
};
