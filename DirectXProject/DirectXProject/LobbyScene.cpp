#include "stdafx.h"

#include "Framework.h"
#include "LobbyScene.h"

using namespace std;

CLobbyScene::CLobbyScene()
{
}
CLobbyScene::~CLobbyScene()
{
	ReleaseObjects();
}

void CLobbyScene::Update(float fTimeElapsed)
{
}

void CLobbyScene::Render()
{
}

bool CLobbyScene::OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	switch (nMessageID)
	{
	case WM_KEYDOWN:
		switch (wParam)
		{
		}
		break;

	case WM_KEYUP:
		switch (wParam)
		{
		case VK_RETURN:
			PostQuitMessage(0);
			break;
		case VK_UP:
			break;
		case VK_DOWN:
			break;
		}
		break;
	}
	return false;
}

bool CLobbyScene::OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	switch (nMessageID)
	{
	case WM_RBUTTONDOWN:
		return true;
	case WM_MOUSEMOVE:
		return true;
	}
	return false;
}

bool CLobbyScene::OnProcessingWindowMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
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

void CLobbyScene::BuildObjects(CFramework * myFramework, CSoundManager *pSoundManager, HWND hWnd, CurrentScene tag)
{
	CScene::BuildObjects(myFramework, pSoundManager, hWnd, tag);
}

void CLobbyScene::ReleaseObjects()
{
}