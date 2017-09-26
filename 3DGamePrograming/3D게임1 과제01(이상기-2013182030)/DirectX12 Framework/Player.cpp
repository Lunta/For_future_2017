#include "stdafx.h"

#include "Player.h"
#include "Camera.h"

CPlayer::CPlayer()
{
	m_pCamera = new CCamera();
	m_pCamera->GenerateProjectionMatrix(1.01f, 5000.0f, 60.0f);

	m_xmf3Position = XMFLOAT3A(0.0f, 0.0f, 0.0f);
	m_xmf3Right = XMFLOAT3A(1.0f, 0.0f, 0.0f);
	m_xmf3Up = XMFLOAT3A(0.0f, 1.0f, 0.0f);
	m_xmf3Look = XMFLOAT3A(0.0f, 0.0f, 1.0f);

	m_xmf3CameraOffset = XMFLOAT3A(0.0f, 0.0f, 0.0f);
	m_xmf3Velocity = XMFLOAT3A(0.0f, 0.0f, 0.0f);

	m_fFriction = 125.0f;
	m_fSpeed = 30.0f;
	m_fPitch = 0.0f;
	m_fRoll = 0.0f;
	m_fYaw = 0.0f;
}

CPlayer::~CPlayer()
{
}

void CPlayer::SetPosition(float x, float y, float z)
{
	m_xmf3Position = XMFLOAT3A(x, y, z);

	CGameObject::SetPosition(x, y, z);
}

void CPlayer::SetCameraOffset(XMFLOAT3A& xmf3CameraOffset)
{
	m_xmf3CameraOffset = xmf3CameraOffset;
	m_pCamera->SetLookAt(Vector3::Add(m_xmf3Position, m_xmf3CameraOffset), m_xmf3Position, m_xmf3Up);
	m_pCamera->GenerateViewMatrix();
}

void CPlayer::SetCameraViewport(int xTopLeft, int yTopLeft, int nWidth, int nHeight)
{
	m_pCamera->SetViewport(xTopLeft, yTopLeft, nWidth, nHeight);
}

void CPlayer::SetBulletList(std::list<CGameObject*>* bullet_list)
{
	m_pBulletsList = bullet_list;
}

void CPlayer::SetShootDelay(float delay)
{
	m_fShootDelayCounter = m_fShootDelaySec = delay;
}

void CPlayer::Move(DWORD dwDirection, float fTimeElapsed)
{
	if (dwDirection)
	{
		XMFLOAT3A xmf3Shift = XMFLOAT3A(0, 0, 0);
		XMFLOAT3A xmf3Direction = XMFLOAT3A(0, 0, 0);

		if (dwDirection & DIR_FORWARD) 
			xmf3Direction = Vector3::Add(xmf3Direction, m_xmf3Look, 1.0);
		if (dwDirection & DIR_BACKWARD) 
			xmf3Direction = Vector3::Add(xmf3Direction, m_xmf3Look, -1.0);
		if (dwDirection & DIR_RIGHT) 
			xmf3Direction = Vector3::Add(xmf3Direction, m_xmf3Right, 1.0);
		if (dwDirection & DIR_LEFT) 
			xmf3Direction = Vector3::Add(xmf3Direction, m_xmf3Right, -1.0);
		if (dwDirection & DIR_UP) 
			xmf3Direction = Vector3::Add(xmf3Direction, m_xmf3Up, 1.0);
		if (dwDirection & DIR_DOWN) 
			xmf3Direction = Vector3::Add(xmf3Direction, m_xmf3Up, -1.0);
		if (xmf3Direction.z < 0)
			xmf3Direction.z = 0.0f;
		xmf3Direction = Vector3::Normalize(xmf3Direction);
		xmf3Shift = Vector3::Add(xmf3Shift, xmf3Direction, m_fSpeed * fTimeElapsed);
		Move(xmf3Shift, true);
	}
}

void CPlayer::Move(XMFLOAT3A& xmf3Shift, bool bUpdateVelocity)
{
	if (bUpdateVelocity)
	{
		m_xmf3Velocity = Vector3::Add(m_xmf3Velocity, xmf3Shift);
	}
	else
	{
		m_xmf3Position = Vector3::Add(xmf3Shift, m_xmf3Position);
		m_pCamera->Move(xmf3Shift);
	}
}

void CPlayer::Move(float x, float y, float z)
{
	Move(XMFLOAT3A(x, y, z), false);
}

void CPlayer::Rotate(float fPitch, float fYaw, float fRoll)
{
	m_pCamera->Rotate(fPitch, fYaw, fRoll);
	if (fPitch != 0.0f)
	{
		XMMATRIX mtxRotate = XMMatrixRotationAxis(XMLoadFloat3A(&m_xmf3Right), XMConvertToRadians(fPitch));
		m_xmf3Look = Vector3::TransformNormal(m_xmf3Look, mtxRotate);
		m_xmf3Up = Vector3::TransformNormal(m_xmf3Up, mtxRotate);
	}
	if (fYaw != 0.0f)
	{
		XMMATRIX mtxRotate = XMMatrixRotationAxis(XMLoadFloat3A(&m_xmf3Up), XMConvertToRadians(fYaw));
		m_xmf3Look = Vector3::TransformNormal(m_xmf3Look, mtxRotate);
		m_xmf3Right = Vector3::TransformNormal(m_xmf3Right, mtxRotate);
	}
	if (fRoll != 0.0f)
	{
		XMMATRIX mtxRotate = XMMatrixRotationAxis(XMLoadFloat3A(&m_xmf3Look), XMConvertToRadians(fRoll));
		m_xmf3Up = Vector3::TransformNormal(m_xmf3Up, mtxRotate);
		m_xmf3Right = Vector3::TransformNormal(m_xmf3Right, mtxRotate);
	}

	m_xmf3Look = Vector3::Normalize(m_xmf3Look);
	m_xmf3Right = Vector3::Normalize(Vector3::CrossProduct(m_xmf3Up, m_xmf3Look));
	m_xmf3Up = Vector3::Normalize(Vector3::CrossProduct(m_xmf3Look, m_xmf3Right));
}

void CPlayer::Update(float fTimeElapsed)
{
	m_fShootDelayCounter += fTimeElapsed;

	Move(m_xmf3Velocity, false);

	m_pCamera->Update(this, m_xmf3Position, fTimeElapsed);
	m_pCamera->GenerateViewMatrix();

	XMFLOAT3A xmf3Deceleration = Vector3::Normalize(Vector3::ScalarProduct(m_xmf3Velocity, -1.0f));
	float fLength = Vector3::Length(m_xmf3Velocity);
	float fDeceleration = m_fFriction * fTimeElapsed;
	if (fDeceleration > fLength) fDeceleration = fLength;
	m_xmf3Velocity = Vector3::Add(m_xmf3Velocity, xmf3Deceleration, fDeceleration);
}

void CPlayer::Reset()
{
	m_xmf4x4World = Matrix4x4::Identity();
	m_xmf3MovingDirection = XMFLOAT3A(0.0f, 0.0f, 1.0f);
	m_xmf3RotationAxis = XMFLOAT3A(0.0f, 1.0f, 0.0f);
	m_xmf3Position = XMFLOAT3A(0.0f, 0.0f, m_xmf3Position.z);
	m_xmf3Right = XMFLOAT3A(1.0f, 0.0f, 0.0f);
	m_xmf3Up = XMFLOAT3A(0.0f, 1.0f, 0.0f);
	m_xmf3Look = XMFLOAT3A(0.0f, 0.0f, 1.0f);
}

void CPlayer::Render(HDC hDCFrameBuffer, CCamera *pCamera)
{
	m_xmf4x4World._11 = m_xmf3Right.x; m_xmf4x4World._12 = m_xmf3Right.y; m_xmf4x4World._13 = m_xmf3Right.z;
	m_xmf4x4World._21 = m_xmf3Up.x; m_xmf4x4World._22 = m_xmf3Up.y; m_xmf4x4World._23 = m_xmf3Up.z;
	m_xmf4x4World._31 = m_xmf3Look.x; m_xmf4x4World._32 = m_xmf3Look.y; m_xmf4x4World._33 = m_xmf3Look.z;
	m_xmf4x4World._41 = m_xmf3Position.x; m_xmf4x4World._42 = m_xmf3Position.y; m_xmf4x4World._43 = m_xmf3Position.z;

	m_xmf4x4World = Matrix4x4::Multiply(XMMatrixRotationRollPitchYaw(XMConvertToRadians(90.0f), 0.0f, 0.0f), m_xmf4x4World);

	CGameObject::Render(hDCFrameBuffer, pCamera);
}

void CPlayer::Input(InputKey input, bool bPushLShift)
{
	switch (input)
	{
	case InputKey::KeyUp:
		break;
	case InputKey::Move_FORWARD:
		break;
	case InputKey::Move_BACKWARD:
		break;
	case InputKey::Move_LEFT:
		break;
	case InputKey::Move_RIGHT:
		break;
	case InputKey::Move_UP:
		break;
	case InputKey::Move_DOWN:
		break;
	case InputKey::Shoot:
		if (m_fShootDelayCounter < m_fShootDelaySec) break;

		CGameObject* Bullet = new CGameObject();

		if(m_pBulletsList->size() == 0)
			m_pObjectBulletMesh = new CCubeMesh(0.5f, 0.5f, 0.5f);

		Bullet->SetMesh(m_pObjectBulletMesh);

		Bullet->SetColor(RGB(255, 0, 255));
		Bullet->SetMovingDirection(m_xmf3Look);

		float player_z = m_xmf3Position.z;
		Bullet->SetPosition(m_xmf3Position);
		Bullet->SetRotationAxis(XMFLOAT3A(
			(float)RANDOM_NUM(-1000, 1000),
			(float)RANDOM_NUM(-1000, 1000),
			(float)RANDOM_NUM(-1000, 1000)));
		Bullet->SetRotationSpeed((float)RANDOM_NUM(50, 100));
		Bullet->SetMovingSpeed(100.0f);
		Bullet->SetOOBB(
			XMFLOAT3A(0.0f, 0.0f, 0.0f),
			XMFLOAT3A(0.25f, 0.25f, 0.25f),
			XMFLOAT4A(0.0f, 0.0f, 0.0f, 1.0f));
		m_pBulletsList->push_back(Bullet);
		m_fShootDelayCounter = 0.0f;
		break;
	}
}
