#pragma once
#include "GameObject.h"
#include "Camera.h"

#define DIR_FORWARD		0x01
#define DIR_BACKWARD	0x02
#define DIR_LEFT		0x04
#define DIR_RIGHT		0x08
#define DIR_UP			0x10
#define DIR_DOWN		0x20

class CPlayer : public CGameObject
{
protected:
	XMFLOAT3	m_xmf3Position;
	XMFLOAT3	m_xmf3Right;
	XMFLOAT3	m_xmf3Up;
	XMFLOAT3	m_xmf3Look;

	float		m_fPitch;
	float		m_fYaw;
	float		m_fRoll;

	XMFLOAT3	m_xmf3Velocity;
	XMFLOAT3	m_xmf3Gravity;
	float		m_fMaxVelocityXZ;
	float		m_fMaxVelocityY;
	float		m_fFriction;

	LPVOID		m_pPlayerUpdatedContext;
	LPVOID		m_pCameraUpdatedContext;

	CCamera		*m_pCamera = NULL;

public:
	CPlayer(int nMeshes = 1);
	virtual ~CPlayer();

	virtual void CreateShaderVariables(
		CD3DDeviceIndRes *pd3dDeviceIndRes, ID3D12GraphicsCommandList *pd3dCommandList);
	virtual void ReleaseShaderVariables();
	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList *pd3dCommandList);
	//위치가 바뀔 때마다 호출되는 함수
	virtual void OnPlayerUpdateCallback(float fTimeElapsed) { }
	virtual void OnCameraUpdateCallback(float fTimeElapsed) { }
	//플레이어의 위치와 회전축으로부터 월드 변환 행렬을 생성하는 함수이다.
	virtual void OnPrepareRender();
	//플레이어의 카메라가 3인칭 카메라일 때 플레이어(메쉬)를 렌더링한다.
	virtual void Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera = NULL);
	virtual CCamera* ChangeCamera(
		  CD3DDeviceIndRes *pd3dDeviceIndRes
		, DWORD nNewCameraMode
		, float fTimeElapsed);	

	XMFLOAT3	GetPosition() { return(m_xmf3Position); }
	XMFLOAT3	GetLookVector() { return(m_xmf3Look); }
	XMFLOAT3	GetUpVector() { return(m_xmf3Up); }
	XMFLOAT3	GetRightVector() { return(m_xmf3Right); }
	XMFLOAT3&	GetVelocity() { return(m_xmf3Velocity); }
	float		GetYaw() { return(m_fYaw); }
	float		GetPitch() { return(m_fPitch); }
	float		GetRoll() { return(m_fRoll); }
	CCamera*	GetCamera() { return(m_pCamera); }

	/*플레이어의 위치를 xmf3Position 위치로 설정한다.
	xmf3Position 벡터에서 현재 플레이어의 위치 벡터를 빼면
	현재 플레이어의 위치에서 xmf3Position 방향으로의 벡터가 된다.
	현재 플레이어의 위치에서 이 벡터 만큼 이동한다.*/
	void SetPosition(XMFLOAT3& xmf3Position);
	void SetVelocity(XMFLOAT3& xmf3Velocity) { m_xmf3Velocity = xmf3Velocity; }
	void SetGravity(XMFLOAT3& xmf3Gravity) { m_xmf3Gravity = xmf3Gravity; }
	void SetMaxVelocityXZ(float fMaxVelocity) { m_fMaxVelocityXZ = fMaxVelocity; }
	void SetMaxVelocityY(float fMaxVelocity) { m_fMaxVelocityY = fMaxVelocity; }
	void SetFriction(float fFriction) { m_fFriction = fFriction; }
	void SetPlayerUpdatedContext(LPVOID pContext) { m_pPlayerUpdatedContext = pContext; }
	void SetCameraUpdatedContext(LPVOID pContext) { m_pCameraUpdatedContext = pContext; }
	void SetCamera(CCamera *pCamera) { m_pCamera = pCamera; }

	void Move(DWORD dwDirection, float fDistance, bool bUpdateVelocity = false);
	void Move(const XMFLOAT3& xmf3Shift, bool bVelocity = false);
	void Move(float fxOffset = 0.0f, float fyOffset = 0.0f, float fzOffset = 0.0f);

	void Rotate(float x, float y, float z);

	void Update(float fTimeElapsed);
	void Reset();

	CCamera* OnChangeCamera(DWORD nNewCameraMode, DWORD nCurrentCameraMode);
};

class CAirplanePlayer : public CPlayer
{
public:
	CAirplanePlayer(CD3DDeviceIndRes *pd3dDeviceIndRes, ID3D12GraphicsCommandList *pd3dCommandList,
		ID3D12RootSignature *pd3dGraphicsRootSignature, int nMeshes = 1);
	virtual ~CAirplanePlayer();

	virtual void OnPrepareRender();
	virtual CCamera *ChangeCamera(CD3DDeviceIndRes *pd3dDeviceIndRes, DWORD nNewCameraMode, float fTimeElapsed);
};

class CTerrainPlayer : public CPlayer
{
public:
	CTerrainPlayer(CD3DDeviceIndRes *pd3dDeviceIndRes, ID3D12GraphicsCommandList *pd3dCommandList,
		ID3D12RootSignature *pd3dGraphicsRootSignature, void *pContext, int nMeshes = 1);
	virtual ~CTerrainPlayer();
	virtual CCamera *ChangeCamera(
		CD3DDeviceIndRes *pd3dDeviceIndRes, DWORD nNewCameraMode, float fTimeElapsed);
	virtual void OnPlayerUpdateCallback(float fTimeElapsed);
	virtual void OnCameraUpdateCallback(float fTimeElapsed);
};