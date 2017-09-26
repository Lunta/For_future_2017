#pragma once
#include "Mesh.h"
#include "Camera.h"

class CShader;

class CGameObject
{
public:
	typedef enum {
		Wall,
		Player,
		RedCube,
		GreenCube,
		BlueCube,
		WhiteCube,
		Bullet,
		Particle,
		Count
	}ObjectType;

public:
	CGameObject();
	virtual ~CGameObject();

private:
	int						m_nReferences = 0;
	bool					m_bActive;

public:
	void AddRef() { m_nReferences++; }
	void Release() { if (--m_nReferences <= 0) delete this; }

	void Awake() { m_bActive = true; }
	void Sleep() { m_bActive = false; }
	bool IsActive() const { return m_bActive; }

protected:
	ObjectType				m_Tag;

	XMFLOAT4X4				m_xmf4x4World;
	CMesh					*m_pMesh;
	CShader					*m_pShader;

	BoundingOrientedBox		m_xmOOBB;
	BoundingOrientedBox		m_xmOOBBTransformed;
	CGameObject				*m_pCollider;

	bool					m_bUpdatedWorldMtx;

public:
	virtual void SetMesh(CMesh *pMesh);
	virtual void SetShader(CShader *pShader);
	// 상수 버퍼
	virtual void CreateShaderVariables(
		ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList);
	virtual void ReleaseShaderVariables();
	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList *pd3dCommandList);
	virtual void OnPrepareRender();
	virtual void Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera);
	virtual void Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera, 
		UINT nInstances);
	virtual void Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera, 
		UINT nInstances, D3D12_VERTEX_BUFFER_VIEW d3dInstancingBufferView);
	virtual void Animate(float fTimeElapsed);
	virtual void Rotate(XMFLOAT3 *pxmf3Axis, float fAngle);
	
	// 게임 객체의 월드 변환 행렬에서 위치 벡터와 방향(x-축, y-축, z-축) 벡터를 반환한다.
	ObjectType	GetObjectTag() const { return m_Tag; }
	XMFLOAT4X4	GetWorldMtx();
	XMFLOAT3	GetPosition();
	XMFLOAT3	GetLook();
	XMFLOAT3	GetUp();
	XMFLOAT3	GetRight();
	const BoundingOrientedBox& GetOOBB() const { return m_xmOOBB ; }
	CGameObject* GetCollider() const { return m_pCollider; }

	// 게임 객체의 타입을 설정한다.
	void SetObjectType(ObjectType tag) { m_Tag = tag; }
	// OOBB를 설정한다.
	void SetOOBB(const BoundingOrientedBox& OOBB) { m_xmOOBB = m_xmOOBBTransformed = OOBB; }
	// 게임 객체의 위치를 설정한다.
	void SetPosition(float x, float y, float z);
	void SetPosition(XMFLOAT3 xmf3Position);
	void SetCollider(CGameObject* pCollider) { m_pCollider  = pCollider; }

	// 게임 객체가 카메라에 보인는 가를 검사한다.
	bool IsVisible(CCamera *pCamera = NULL);

	// 게임 객체를 로컬 x-축, y-축, z-축 방향으로 이동한다.
	void MoveStrafe(float fDistance = 1.0f);
	void MoveUp(float fDistance = 1.0f);
	void MoveForward(float fDistance = 1.0f);
	// 게임 객체를 임의방향(월드)으로 이동한다.
	void Move(XMFLOAT3& vDirection, float fSpeed);
	// 게임 객체를 회전(x-축, y-축, z-축)한다.
	void Rotate(float fPitch = 10.0f, float fYaw = 10.0f, float fRoll = 10.0f);

	void ReleaseUploadBuffers();
};

class CRotatingObject : public CGameObject
{
public:
	CRotatingObject();
	virtual ~CRotatingObject();

private:
	XMFLOAT3				m_xmf3MovingDirection;
	float					m_fMovingSpeed;
	float					m_fMovingRange;

	XMFLOAT3				m_xmf3RotationAxis;
	float					m_fRotationSpeed;

public:
	virtual void Animate(float fTimeElapsed) override;

	void SetMovingDirection(XMFLOAT3& xmf3MovingDirection) { m_xmf3MovingDirection = Vector3::Normalize(xmf3MovingDirection); }
	void SetMovingSpeed(float fSpeed) { m_fMovingSpeed = fSpeed; }
	void SetMovingRange(float fRange) { m_fMovingRange = fRange; }

	void SetRotationSpeed(float fRotationSpeed) { m_fRotationSpeed = fRotationSpeed; }
	void SetRotationAxis(XMFLOAT3 xmf3RotationAxis) { m_xmf3RotationAxis = xmf3RotationAxis; }

	XMFLOAT3& GetMovingDirection() { return m_xmf3MovingDirection; }
	float& GetMovingSpeed() { return m_fMovingSpeed; }
	float& GetMovingRange() { return m_fMovingRange; }
};