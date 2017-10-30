#include "stdafx.h"

#include "Framework.h"
#include "MainScene.h"

using namespace std;

CMainScene::CMainScene() : num(0)
{

}


CMainScene::~CMainScene()
{
}

void CMainScene::Update(float fTimeElapsed)
{
	printf("/rThe number of Objects: %d", num);

	m_CreateUnit_Counter += fTimeElapsed;
	if (m_CreateUnit_Counter > 1)
	{
		m_CreateUnit_Counter = 0;
		BuildUnit();
	}

	for (auto p1 = m_UnitList_A.begin(); p1 != m_UnitList_A.end(); p1 = p1->GetNext())
	{
		p1->m_Elem.Update(fTimeElapsed, m_UnitList_B);
	}
	for (auto p2 = m_UnitList_B.begin(); p2 != m_UnitList_B.end(); p2 = p2->GetNext())
	{
		p2->m_Elem.Update(fTimeElapsed, m_UnitList_A);
	}

	m_UnitList_A.remove_if([](const decltype(m_UnitList_A)::type& data)-> bool {
		return (data.Get_UnitStatus_HP() <= 0);
	});
	m_UnitList_B.remove_if([](const decltype(m_UnitList_B)::type& data)-> bool {
		return (data.Get_UnitStatus_HP() <= 0);
	});

}

void CMainScene::Render(ID2D1HwndRenderTarget* pRendertarget)
{
	

	for (auto p1 = m_UnitList_A.begin(); p1 != m_UnitList_A.end(); p1 = p1->GetNext())
	{
		p1->m_Elem.Draw(pRendertarget, 0);
	}
	for (auto p2 = m_UnitList_B.begin(); p2 != m_UnitList_B.end(); p2 = p2->GetNext())
	{
		p2->m_Elem.Draw(pRendertarget, 1);
	}
}

bool CMainScene::OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	switch (nMessageID)
	{
	case WM_KEYDOWN:
		return true;
	case WM_KEYUP:
		return true;
	}
	return false;
}

bool CMainScene::OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	switch (nMessageID)
	{
	case WM_LBUTTONDOWN:
		return true;
	case WM_RBUTTONDOWN:
		break;
	case WM_MOUSEMOVE:
		return true;
	}
	return false;
}

bool CMainScene::OnProcessingWindowMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	switch (nMessageID)
	{
	case WM_SIZE:
		break;
	case WM_DESTROY:
		break;
	}
	return false;
}

void CMainScene::BuildObjects(CFramework * myFramework, HWND hWnd, CurrentScene tag)
{
	CScene::BuildObjects(myFramework, hWnd, tag);

}

void CMainScene::ReleaseObjects()
{
}

void CMainScene::BuildUnit()
{
	RECT rc;
	rc = { m_rcScene.left, m_rcScene.top, m_rcScene.left + 100, m_rcScene.bottom };
	m_UnitList_A.push_front(CUnit(rc));
	rc = { m_rcScene.right - 100, m_rcScene.top, m_rcScene.right, m_rcScene.bottom };
	m_UnitList_B.push_front(CUnit(rc));
	num += 2;
}
