#pragma once
#include "GameTimer.h"
#include "Shader.h"
#include "Camera.h"

#define BulletDelay 0.1f
#define REPULSIVE_FORCE 50.0f

class CGameFramework;
class CScene
{
public:
	typedef enum {
		Background,
		Objects,
		Bullet,
		Particle,
		Count
	} ObjectTag;

public:
	CScene();
	~CScene();

	void SetFramework(CGameFramework* pFramework) { m_pFramework = pFramework; }
	void SetHWNDAndHInstance(HWND hWnd, HINSTANCE hInstance);
	void SetPlayer(CPlayer* pPlayer) { m_pPlayer = pPlayer; }

	ID3D12RootSignature* CreateGraphicsRootSignature(ID3D12Device *pd3dDevice);
	ID3D12RootSignature* GetGraphicsRootSignature();

	CHeightMapTerrain *GetTerrain() { return(m_pTerrain); }

	void BuildObjects(
		ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList);
	void ReleaseObjects();

	void Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera);
	bool ProcessInput(float fTimeElapsed);
	void AnimateObjects(float fTimeElapsed);
	void PhysicsProcessing(float fTimeElapsed);

	void ReleaseUploadBuffers();

	bool OnProcessingMouseMessage(
		HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	bool OnProcessingKeyboardMessage(
		HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);

	//씬의 모든 게임 객체들에 대한 마우스 픽킹을 수행한다.
	CGameObject *PickObjectPointedByCursor(int xClient, int yClient, CCamera *pCamera);

	void ShootBullet();
	void ResetObjects(CRotatingObject* Object);
	void PopParticles(XMFLOAT3& pos, int nParticles = 50);

protected:
	HINSTANCE					m_hInstance;
	HWND						m_hWnd;

	CGameFramework				*m_pFramework = NULL;

	CPlayer						*m_pPlayer = NULL;
	Maze						m_Maze;
	int							m_nCurrentBulletIdx = 0;
	int							m_nCurrentParticleIdx = 0;

	int							m_nShaders = ObjectTag::Count;
	CInstancingShader			*m_pShaders = NULL;
	CHeightMapTerrain			*m_pTerrain = NULL;
	ComPtr<ID3D12RootSignature> m_pd3dGraphicsRootSignature = NULL;

	int							m_pnObjects[ObjectTag::Count];
	CGameObject					***m_pppObjects = NULL;
	CGameObject					*m_pRoofObject = NULL;
	CGameObject					*m_pSelectedObject = NULL;

	POINT						m_ptOldCursorPos;
	float						m_fRestartCounter;
	float						m_fBulletTimer;

	bool						m_bGameOver;

};