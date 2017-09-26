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
	// ��� ����
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
	
	// ���� ��ü�� ���� ��ȯ ��Ŀ��� ��ġ ���Ϳ� ����(x-��, y-��, z-��) ���͸� ��ȯ�Ѵ�.
	ObjectType	GetObjectTag() const { return m_Tag; }
	XMFLOAT4X4	GetWorldMtx();
	XMFLOAT3	GetPosition();
	XMFLOAT3	GetLook();
	XMFLOAT3	GetUp();
	XMFLOAT3	GetRight();
	const BoundingOrientedBox& GetOOBB() const { return m_xmOOBB ; }
	CGameObject* GetCollider() const { return m_pCollider; }

	// ���� ��ü�� Ÿ���� �����Ѵ�.
	void SetObjectType(ObjectType tag) { m_Tag = tag; }
	// OOBB�� �����Ѵ�.
	void SetOOBB(const BoundingOrientedBox& OOBB) { m_xmOOBB = m_xmOOBBTransformed = OOBB; }
	// ���� ��ü�� ��ġ�� �����Ѵ�.
	void SetPosition(float x, float y, float z);
	void SetPosition(XMFLOAT3 xmf3Position);
	void SetCollider(CGameObject* pCollider) { m_pCollider  = pCollider; }

	// ���� ��ü�� ī�޶� ���δ� ���� �˻��Ѵ�.
	bool IsVisible(CCamera *pCamera = NULL);

	// ���� ��ü�� ���� x-��, y-��, z-�� �������� �̵��Ѵ�.
	void MoveStrafe(float fDistance = 1.0f);
	void MoveUp(float fDistance = 1.0f);
	void MoveForward(float fDistance = 1.0f);
	// ���� ��ü�� ���ǹ���(����)���� �̵��Ѵ�.
	void Move(XMFLOAT3& vDirection, float fSpeed);
	// ���� ��ü�� ȸ��(x-��, y-��, z-��)�Ѵ�.
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