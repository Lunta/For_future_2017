#pragma once
#include "Scene.h"
#include "Unit.h"

class CMainScene :public CScene
{
protected:
	WarpList<CUnit> m_UnitList_A;
	WarpList<CUnit> m_UnitList_B;

	float m_CreateUnit_Counter{ 0 };
	unsigned int num;

public:
	CMainScene();
	~CMainScene();

	virtual void Update(float fTimeElapsed);
	virtual void Render(ID2D1HwndRenderTarget* pRendertarget);

	// �Լ� ȣ�� ���Ŀ��� ó���� ���ϸ� false ��ȯ
	virtual bool OnProcessingKeyboardMessage	(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	virtual bool OnProcessingMouseMessage		(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	virtual bool OnProcessingWindowMessage		(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);

	virtual void BuildObjects(CFramework* pFramework, HWND hwnd, CurrentScene tag);
	virtual void ReleaseObjects();

	void BuildUnit();
};
