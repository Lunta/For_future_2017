#pragma once

class CPlayer;

struct VIEWPORT
{
	int						m_xStart;
	int						m_yStart;
	int						m_nWidth;
	int						m_nHeight;
};

class CCamera
{
public:
	CCamera();
	virtual ~CCamera();

	XMFLOAT3A				m_xmf3Position;
	XMFLOAT3A				m_xmf3Right;
	XMFLOAT3A				m_xmf3Up;
	XMFLOAT3A				m_xmf3Look;

	XMFLOAT4X4A				m_xmf4x4View;
	XMFLOAT4X4A				m_xmf4x4Projection;
	XMFLOAT4X4A				m_xmf4x4ViewProject;

	VIEWPORT				m_Viewport;

	void GenerateViewMatrix();
	void GenerateProjectionMatrix(float fNearPlaneDistance, float fFarPlaneDistance, float fFOVAngle);
	void SetViewport(int xStart, int yStart, int nWidth, int nHeight);

	void SetLookAt(XMFLOAT3A& xmf3LookAt, XMFLOAT3A& xmf3Up);
	void SetLookAt(XMFLOAT3A& vPosition, XMFLOAT3A& xmf3LookAt, XMFLOAT3A& xmf3Up);

	void Move(XMFLOAT3A& xmf3Shift);
	void Move(float x, float y, float z);
	void Rotate(float fPitch = 0.0f, float fYaw = 0.0f, float fRoll = 0.0f);
	void Update(CPlayer *pPlayer, XMFLOAT3A& xmf3LookAt, float fTimeElapsed = 0.016f);
};
