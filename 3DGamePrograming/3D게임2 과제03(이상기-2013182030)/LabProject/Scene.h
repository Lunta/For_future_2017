#pragma once
#include "GameTimer.h"
#include "Shader.h"
#include "Camera.h"

#define BulletDelay				0.1f
#define REPULSIVE_FORCE			50.0f
#define NUM_TREES				2000
#define NUM_GRASSES				10000

struct LIGHT
{
	XMFLOAT4				m_xmf4Ambient;
	XMFLOAT4				m_xmf4Diffuse;
	XMFLOAT4				m_xmf4Specular;
	XMFLOAT3				m_xmf3Position;
	float 					m_fFalloff;
	XMFLOAT3				m_xmf3Direction;
	float 					m_fTheta; //cos(m_fTheta)
	XMFLOAT3				m_xmf3Attenuation;
	float					m_fPhi; //cos(m_fPhi)
	bool					m_bEnable;
	int						m_nType;
	float					m_fRange;
	float					padding;
};

struct LIGHTS
{
	LIGHT					m_pLights[MAX_LIGHTS];
	XMFLOAT4				m_xmf4GlobalAmbient;
};

struct MATERIAL
{
	XMFLOAT4				m_xmf4Ambient;
	XMFLOAT4				m_xmf4Diffuse;
	XMFLOAT4				m_xmf4Specular; //(r,g,b,a=power)
	XMFLOAT4				m_xmf4Emissive;
};

struct MATERIALS
{
	MATERIAL				m_pReflections[MAX_MATERIALS];
};

class CGameFramework;
class CScene
{
public:
	CScene();
	~CScene();

	void SetFramework(CGameFramework* pFramework) { m_pFramework = pFramework; }
	void SetHWNDAndHInstance(HWND hWnd, HINSTANCE hInstance);
	void SetPlayer(CPlayer* pPlayer) { m_pPlayer = pPlayer; }

	ID3D12RootSignature* CreateGraphicsRootSignature(
		CD3DDeviceIndRes* pd3dDeviceIndRes);
	ID3D12RootSignature* GetGraphicsRootSignature();

	CHeightMapTerrain *GetTerrain() { return(m_pTerrain); }

	virtual void CreateShaderVariables(CD3DDeviceIndRes* pd3dDeviceIndRes, ID3D12GraphicsCommandList *pd3dCommandList);
	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList *pd3dCommandList);
	virtual void ReleaseShaderVariables();

	void BuildLightsAndMaterials();
	void BuildObjects(
		CD3DDeviceIndRes* pd3dDeviceIndRes
		, ID3D12GraphicsCommandList *pd3dCommandList);
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

protected:
	HINSTANCE								m_hInstance;
	HWND									m_hWnd;

	CGameFramework							*m_pFramework = NULL;

	ComPtr<ID3D12RootSignature>				m_pd3dGraphicsRootSignature = NULL;

	CShader									*m_pShaders = NULL;
	int										m_nShaders = 0;

	CHeightMapTerrain						*m_pTerrain = NULL;
	CSkyBox									*m_pSkyBox = NULL;
	CBillBoardShader						*m_pTrees = NULL;
	CBillBoardShader						*m_pGrasses = NULL;

	LIGHTS									*m_pLights = NULL;
	ComPtr<ID3D12Resource>					m_pd3dcbLights = NULL;
	LIGHTS									*m_pcbMappedLights = NULL;

	int										m_nMaterials = 0;
	MATERIALS								*m_pMaterials = NULL;
	ComPtr<ID3D12Resource>					m_pd3dcbMaterials = NULL;
	MATERIAL								*m_pcbMappedMaterials = NULL;

	int										m_nObjects = 0;
	CGameObject								**m_ppObjects = NULL;

	POINT									m_ptOldCursorPos;

	CPlayer									*m_pPlayer = NULL;

	int										m_nCurrentBulletIdx = 0;
	int										m_nCurrentParticleIdx = 0;

	float									m_fRestartCounter;
	float									m_fBulletTimer;

	bool									m_bGameOver;
};