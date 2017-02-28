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

	// �Լ� ȣ�� ���Ŀ��� ó���� ���ϸ� false ��ȯ
	virtual bool OnProcessingKeyboardMessage	(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	virtual bool OnProcessingMouseMessage		(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	virtual bool OnProcessingWindowMessage		(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);

	virtual void BuildObjects(CFramework* pFramework, CSoundManager *pSoundManager, HWND hwnd, CurrentScene tag);
	virtual void ReleaseObjects();
};
