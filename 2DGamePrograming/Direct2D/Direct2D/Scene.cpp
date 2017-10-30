#include "stdafx.h"

#include "Framework.h"
#include "Scene.h"

using namespace std;

CScene::CScene()
{
}
CScene::~CScene()
{
}

bool CScene::OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	switch (wParam)
	{
	case WM_KEYDOWN:
		return true;
	case WM_KEYUP:
		return true;
	}
	return false;
}

bool CScene::OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	switch (wParam)
	{
	case WM_LBUTTONDOWN:
		return true;
	case WM_MOUSEMOVE:
		return true;
	}
	return false;
}

bool CScene::OnProcessingWindowMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	switch (wParam)
	{
	case WM_SIZE:
		break;
	case WM_DESTROY:
		break;
	}
	return false;
}

void CScene::BuildObjects(CFramework * myFramework, HWND hWnd, CurrentScene tag)
{
	m_pFramework = myFramework;
	m_hWnd = hWnd;

	m_Tag = tag;

	::GetClientRect(m_hWnd, &m_rcScene);

}

void CScene::ReleaseObjects()
{
}
