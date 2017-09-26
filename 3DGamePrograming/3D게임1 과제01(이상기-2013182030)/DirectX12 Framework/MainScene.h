#pragma once
#include "Scene.h"
#include <list>
#include <vector>
#include <iterator>

class CPlayer;
class CGameObject;
class CCubeMesh;

class CMainScene :public CScene
{
protected:
	
	CPlayer						*m_pPlayer;

	int							m_nObjects;
	std::vector<CGameObject*>	m_vecObjects;

	std::vector<CGameObject*>	m_vecWalls;
	XMFLOAT4A					m_pxmf4WallPlanes[6];

	std::list<CGameObject*>		m_BulletsList;

	std::list<CGameObject*>		m_ParticlesList;
	CCubeMesh					*m_pObjectParticleMesh;

	bool						m_bGameOver;
	float						m_fRestartCounter;

public:
	CMainScene();
	virtual ~CMainScene();

	virtual void Animate(float fTimeElapsed);
	virtual void Update(float fTimeElapsed);
	virtual void Draw(HDC hdc);

	// 함수 호출 이후에도 처리를 원하면 false 반환
	virtual bool OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	virtual bool OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	virtual bool OnProcessingWindowMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);

	virtual void BuildObjects(CFramework* pFramework, CSoundManager *pSoundManager, HWND hwnd, CurrentScene tag);
	virtual void ReleaseObjects();

	void BuildParticles(XMFLOAT3A& xmf3Position, int nParticles = 50);
};

