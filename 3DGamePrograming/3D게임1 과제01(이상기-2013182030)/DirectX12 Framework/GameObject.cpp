#include "stdafx.h"

#include "GameObject.h"
#include "Camera.h"

CGameObject::CGameObject()
{
	m_pMesh = NULL;
	m_xmf4x4World = Matrix4x4::Identity();

	m_dwColor = RGB(0, 0, 0);

	m_xmf3MovingDirection = XMFLOAT3A(0.0f, 0.0f, 1.0f);
	m_fMovingSpeed = 0.0f;
	m_fMovingRange = 0.0f;

	m_xmf3RotationAxis = XMFLOAT3A(0.0f, 1.0f, 0.0f);
	m_fRotationSpeed = 0.05f;

	m_bStiff = false;
	m_fStiffTime = 0.0f;
	m_fStiffCounter = 0.0f;
}

CGameObject::CGameObject(CMesh *pMesh) : CGameObject()
{
	m_pMesh = pMesh;
}

CGameObject::~CGameObject()
{
	if (m_pMesh) m_pMesh->Release();
}

void CGameObject::SetPosition(float x, float y, float z)
{
	m_xmf4x4World._41 = x;
	m_xmf4x4World._42 = y;
	m_xmf4x4World._43 = z;
}

void CGameObject::SetPosition(XMFLOAT3A& xmf3Position)
{
	m_xmf4x4World._41 = xmf3Position.x;
	m_xmf4x4World._42 = xmf3Position.y;
	m_xmf4x4World._43 = xmf3Position.z;
}

XMFLOAT3A CGameObject::GetPosition()
{
	return(XMFLOAT3A(m_xmf4x4World._41, m_xmf4x4World._42, m_xmf4x4World._43));
}

XMFLOAT3A CGameObject::GetLook()
{
	XMFLOAT3A xmf3LookAt(m_xmf4x4World._31, m_xmf4x4World._32, m_xmf4x4World._33);
	xmf3LookAt = Vector3::Normalize(xmf3LookAt);
	return(xmf3LookAt);
}

XMFLOAT3A CGameObject::GetUp()
{
	XMFLOAT3A xmf3Up(m_xmf4x4World._21, m_xmf4x4World._22, m_xmf4x4World._23);
	xmf3Up = Vector3::Normalize(xmf3Up);
	return(xmf3Up);
}

XMFLOAT3A CGameObject::GetRight()
{
	XMFLOAT3A xmf3Right(m_xmf4x4World._11, m_xmf4x4World._12, m_xmf4x4World._13);
	xmf3Right = Vector3::Normalize(xmf3Right);
	return(xmf3Right);
}

void CGameObject::MoveStrafe(float fDistance)
{
	XMFLOAT3A xmf3Position = GetPosition();
	XMFLOAT3A xmf3Right = GetRight();
	xmf3Position = Vector3::Add(xmf3Position, Vector3::ScalarProduct(xmf3Right, fDistance));
	CGameObject::SetPosition(xmf3Position);
}

void CGameObject::MoveUp(float fDistance)
{
	XMFLOAT3A xmf3Position = GetPosition();
	XMFLOAT3A xmf3Up = GetUp();
	xmf3Position = Vector3::Add(xmf3Position, Vector3::ScalarProduct(xmf3Up, fDistance));
	CGameObject::SetPosition(xmf3Position);
}

void CGameObject::MoveForward(float fDistance)
{
	XMFLOAT3A xmf3Position = GetPosition();
	XMFLOAT3A xmf3LookAt = GetLook();
	xmf3Position = Vector3::Add(xmf3Position, Vector3::ScalarProduct(xmf3LookAt, fDistance));
	CGameObject::SetPosition(xmf3Position);
}

void CGameObject::Rotate(float fPitch, float fYaw, float fRoll)
{
	XMFLOAT4X4A mtxRotate = Matrix4x4::RotationYawPitchRoll(fYaw, fPitch, fRoll);
	m_xmf4x4World = Matrix4x4::Multiply(mtxRotate, m_xmf4x4World);
}

void CGameObject::Rotate(XMFLOAT3A& xmf3RotationAxis, float fAngle)
{
	XMFLOAT4X4A mtxRotate = Matrix4x4::RotationAxis(xmf3RotationAxis, fAngle);
	m_xmf4x4World = Matrix4x4::Multiply(mtxRotate, m_xmf4x4World);
}

void CGameObject::Move(XMFLOAT3A& vDirection, float fSpeed)
{
	SetPosition(m_xmf4x4World._41 + vDirection.x * fSpeed, m_xmf4x4World._42 + vDirection.y * fSpeed, m_xmf4x4World._43 + vDirection.z * fSpeed);
}

void CGameObject::Animate(float fTimeElapsed)
{
	if (m_bStiff)
	{
		m_fStiffCounter += fTimeElapsed;
		if (m_fStiffCounter > m_fStiffTime)
		{
			m_fStiffCounter = 0.0f;
			m_bStiff = false;
		}
	}
	if (m_fRotationSpeed != 0.0f) Rotate(m_xmf3RotationAxis, m_fRotationSpeed * fTimeElapsed);
	if (m_fMovingSpeed != 0.0f) Move(m_xmf3MovingDirection, m_fMovingSpeed * fTimeElapsed);

	m_xmOOBBTransformed.Transform(m_xmOOBB, XMLoadFloat4x4A(&m_xmf4x4World));
	XMStoreFloat4(&m_xmOOBBTransformed.Orientation, XMQuaternionNormalize(XMLoadFloat4(&m_xmOOBBTransformed.Orientation)));
}

void CGameObject::Render(HDC hDCFrameBuffer, CCamera *pCamera)
{
	XMFLOAT4X4A xm4x4Transform = Matrix4x4::Multiply(m_xmf4x4World, pCamera->m_xmf4x4ViewProject);
	HPEN hPen = ::CreatePen(PS_SOLID, 0, m_dwColor);
	HPEN hOldPen = (HPEN)::SelectObject(hDCFrameBuffer, hPen);
	if (m_pMesh) m_pMesh->Render(hDCFrameBuffer, xm4x4Transform, pCamera);
	::SelectObject(hDCFrameBuffer, hOldPen);
	::DeleteObject(hPen);
}
