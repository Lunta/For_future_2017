#pragma once

#include "Shader.h"

#define BulletDelay 0.1f
#define REPULSIVE_FORCE 60.0f

class CGameObject;
class CCamera;
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

protected:
	HINSTANCE			m_hInstance;
	HWND				m_hWnd;

	CGameFramework		*m_pFramework;

	CPlayer				*m_pPlayer;
	int					m_nCurrentBulletIdx;
	int					m_nCurrentParticleIdx;
	XMFLOAT4			m_pxmf4WallPlanes[6];

	int					m_pnObjects[ObjectTag::Count];
	CGameObject			***m_pppObjects;
	//배치(Batch) 처리를 하기 위하여 씬을 셰이더들의 리스트로 표현한다.
	int					m_nShaders = 0;
	CInstancingShader	*m_pShaders = NULL;
	ID3D12RootSignature *m_pd3dGraphicsRootSignature = NULL;

	POINT				m_ptOldCursorPos;
	float				m_fRestartCounter;
	float				m_fBulletTimer;

	bool				m_bGameOver;

public:
	CScene();
	~CScene();

	void SetFramework(CGameFramework* pFramework) { m_pFramework = pFramework; }
	void SetHWNDAndHInstance(HWND hWnd, HINSTANCE hInstance);
	void SetPlayer(CPlayer* pPlayer) { m_pPlayer = pPlayer; }

	ID3D12RootSignature* CreateGraphicsRootSignature(ID3D12Device *pd3dDevice);
	ID3D12RootSignature* GetGraphicsRootSignature();

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

	void ShootBullet();
	void ResetObjects(CRotatingObject* Object, float fPlayerPos_Z);
	void PopParticles(XMFLOAT3& pos, int nParticles = 50);
};