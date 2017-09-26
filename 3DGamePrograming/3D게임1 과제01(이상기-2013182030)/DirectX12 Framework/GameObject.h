#pragma once
#include "ModelData.h"
class CCamera;

class CGameObject
{
public:
	CGameObject();
	CGameObject(CMesh *pMesh);

	virtual ~CGameObject();

public:
	CMesh					*m_pMesh;
	XMFLOAT4X4A				m_xmf4x4World;

	BoundingOrientedBox		m_xmOOBB;
	BoundingOrientedBox		m_xmOOBBTransformed;
	CGameObject				*m_pCollider;

	DWORD					m_dwColor;

	XMFLOAT3A				m_xmf3MovingDirection;
	float					m_fMovingSpeed;
	float					m_fMovingRange;

	XMFLOAT3A				m_xmf3RotationAxis;
	float					m_fRotationSpeed;

	bool					m_bStiff;
	float					m_fStiffTime;
	float					m_fStiffCounter;

public:
	void SetMesh(CMesh *pMesh) { m_pMesh = pMesh; if (pMesh) pMesh->AddRef(); }
	void SetColor(DWORD dwColor) { m_dwColor = dwColor; }
	void SetPosition(float x, float y, float z);
	void SetPosition(XMFLOAT3A& xmf3Position);

	void SetOOBB(XMFLOAT3A& xmCenter, XMFLOAT3A& xmExtents, XMFLOAT4A& xmOrientation) { m_xmOOBBTransformed = m_xmOOBB = BoundingOrientedBox(xmCenter, xmExtents, xmOrientation); }

	void SetMovingDirection(XMFLOAT3A& xmf3MovingDirection) { m_xmf3MovingDirection = Vector3::Normalize(xmf3MovingDirection); }
	void SetMovingSpeed(float fSpeed) { m_fMovingSpeed = fSpeed; }
	void SetMovingRange(float fRange) { m_fMovingRange = fRange; }

	void SetRotationAxis(XMFLOAT3A& xmf3RotationAxis) { m_xmf3RotationAxis = Vector3::Normalize(xmf3RotationAxis); }
	void SetRotationSpeed(float fSpeed) { m_fRotationSpeed = fSpeed; }

	void SetStiffTime(float fStifftime) { m_fStiffTime = fStifftime; }

	void MoveStrafe(float fDistance = 1.0f);
	void MoveUp(float fDistance = 1.0f);
	void MoveForward(float fDistance = 1.0f);
	void Move(XMFLOAT3A& vDirection, float fSpeed);

	void Rotate(float fPitch = 10.0f, float fYaw = 10.0f, float fRoll = 10.0f);
	void Rotate(XMFLOAT3A& xmf3Axis, float fAngle);

	void GoStiff() { m_bStiff = true; }
	bool IsStiff() { return m_bStiff; }

	XMFLOAT3A GetPosition();
	XMFLOAT3A GetLook();
	XMFLOAT3A GetUp();
	XMFLOAT3A GetRight();

	virtual void Animate(float fTimeElapsed);
	virtual void Render(HDC hDCFrameBuffer, CCamera *pCamera);
};

