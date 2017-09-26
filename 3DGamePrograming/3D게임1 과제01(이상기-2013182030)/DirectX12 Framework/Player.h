
#pragma once
#include "GameObject.h"
#include <list>

class CPlayer : public CGameObject
{
public:

	enum class InputKey : short
	{
		KeyUp
		, Move_FORWARD
		, Move_BACKWARD
		, Move_LEFT
		, Move_RIGHT
		, Move_UP
		, Move_DOWN
		, Shoot
	};

public:

	XMFLOAT3A					m_xmf3Position;
	XMFLOAT3A					m_xmf3Right;
	XMFLOAT3A					m_xmf3Up;
	XMFLOAT3A					m_xmf3Look;

	XMFLOAT3A					m_xmf3CameraOffset;
	XMFLOAT3A					m_xmf3Velocity;
	float						m_fFriction;

	float           			m_fPitch;
	float           			m_fYaw;
	float           			m_fRoll;

	float						m_fSpeed;

	CCamera						*m_pCamera;

	CCubeMesh					*m_pObjectBulletMesh;
	std::list<CGameObject*>		*m_pBulletsList;
	float						m_fShootDelaySec;
	float						m_fShootDelayCounter;


	CPlayer();
	virtual ~CPlayer();

	void SetPosition(float x, float y, float z);
	void SetCameraOffset(XMFLOAT3A& xmf3CameraOffset);
	void SetCameraViewport(int xTopLeft, int yTopLeft, int nWidth, int nHeight);
	void SetBulletList(std::list<CGameObject*>* bullet_list);
	void SetShootDelay(float delay);

	void Move(DWORD dwDirection, float fTimeElapsed);
	void Move(XMFLOAT3A& xmf3Shift, bool bUpdateVelocity);
	void Move(float x, float y, float z);
	void Rotate(float fPitch = 0.0f, float fYaw = 0.0f, float fRoll = 0.0f);
	void Update(float fTimeElapsed);

	void Reset();

	virtual void Render(HDC hDCFrameBuffer, CCamera *pCamera);

	virtual void Input(InputKey input, bool bPushLShift = false);

};