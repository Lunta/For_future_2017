#pragma once
#include "Mesh.h"
#include "Camera.h"

class CShader;

class CGameObject
{
public:
	typedef enum {
		Background,
		Wall,
		Player,
		Enemy,
		Bullet,
		Particle,
		Count
	}ObjectType;

public:
	CGameObject(int nMeshes = 1);
	virtual ~CGameObject();

private:
	int m_nReferences = 0;
	bool m_bActive = false;

public:
	void AddRef() { m_nReferences++; }
	void Release() { if (--m_nReferences <= 0) delete this; }

	void Awake() { m_bActive = true; }
	void Sleep() { m_bActive = false; }
	bool IsActive() const { return m_bActive; }

protected:
	ObjectType				m_Tag;

	XMFLOAT4X4				m_xmf4x4World;
	//게임 객체는 여러 개의 메쉬를 포함하는 경우 게임 객체가 가지는 메쉬들에 대한 포인터와 그 개수이다.
	int						m_nMeshes = 0;
	CMesh					**m_ppMeshes = NULL;
	CShader					*m_pShader = NULL;

	BoundingOrientedBox		m_xmOOBB;
	BoundingOrientedBox		m_xmOOBBTransformed;
	CGameObject				*m_pCollider;

	bool					m_bUpdatedWorldMtx = true;

public:
	virtual void SetMesh(int nIndex, CMesh *pMesh);
	virtual void SetShader(CShader *pShader);
	//상수 버퍼
	virtual void CreateShaderVariables(
		CD3DDeviceIndRes *pd3dDeviceIndRes, ID3D12GraphicsCommandList *pd3dCommandList);
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
	
	//게임 객체의 월드 변환 행렬에서 위치 벡터와 방향(x-축, y-축, z-축) 벡터를 반환한다.
	ObjectType	GetObjectTag() const { return m_Tag; }
	XMFLOAT4X4	GetWorldMtx();
	XMFLOAT3	GetPosition();
	XMFLOAT3	GetLook();
	XMFLOAT3	GetUp();
	XMFLOAT3	GetRight();
	const BoundingOrientedBox& GetOOBB() const { return m_xmOOBB; }
	CGameObject* GetCollider() const { return m_pCollider; }

	// 게임 객체의 타입을 설정한다.
	void SetObjectType(ObjectType tag) { m_Tag = tag; }
	// OOBB를 설정한다.
	void SetOOBB(const BoundingOrientedBox& OOBB) { m_xmOOBB = m_xmOOBBTransformed = OOBB; }
	// 게임 객체의 위치를 설정한다.
	void SetPosition(float x, float y, float z);
	void SetPosition(XMFLOAT3 xmf3Position);
	void SetCollider(CGameObject* pCollider) { m_pCollider = pCollider; }

	//게임 객체를 로컬 x-축, y-축, z-축 방향으로 이동한다.
	void MoveStrafe(float fDistance = 1.0f);
	void MoveUp(float fDistance = 1.0f);
	void MoveForward(float fDistance = 1.0f);
	void Move(XMFLOAT3& vDirection, float fSpeed);
	//게임 객체를 회전(x-축, y-축, z-축)한다.
	void Rotate(float fPitch = 10.0f, float fYaw = 10.0f, float fRoll = 10.0f);

	void ReleaseUploadBuffers();

	//게임 객체가 카메라에 보인는 가를 검사한다.
	bool IsVisible(CCamera *pCamera = NULL);
	//모델 좌표계의 픽킹 광선을 생성한다.
	void GenerateRayForPicking(XMFLOAT3& xmf3PickPosition, XMFLOAT4X4& xmf4x4View,
		XMFLOAT3 *pxmf3PickRayOrigin, XMFLOAT3 *pxmf3PickRayDirection);
	//카메라 좌표계의 한 점에 대한 모델 좌표계의 픽킹 광선을 생성하고 객체와의 교차를 검사한다.
	int PickObjectByRayIntersection(XMFLOAT3& xmf3PickPosition, XMFLOAT4X4& xmf4x4View,
		float *pfHitDistance);
};

class CRotatingObject : public CGameObject
{
public:
	CRotatingObject(int nMeshes = 1);
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

class CHeightMapTerrain : public CGameObject
{
public:
	CHeightMapTerrain(
		CD3DDeviceIndRes *pd3dDeviceIndRes
		, ID3D12GraphicsCommandList *pd3dCommandList
		, ID3D12RootSignature *pd3dGraphicsRootSignature
		, LPCTSTR pFileName, int nWidth, int nLength
		, int nBlockWidth, int nBlockLength
		, XMFLOAT3 xmf3Scale, XMFLOAT4 xmf4Color);
	virtual ~CHeightMapTerrain();
private:
	//지형의 높이 맵으로 사용할 이미지이다.
	CHeightMapImage *m_pHeightMapImage;
	//높이 맵의 가로와 세로 크기이다.
	int m_nWidth;
	int m_nLength;
	//지형을 실제로 몇 배 확대할 것인가를 나타내는 스케일 벡터이다.
	XMFLOAT3 m_xmf3Scale;

public:
	//지형의 높이를 계산하는 함수이다(월드 좌표계). 높이 맵의 높이에 스케일의 y를 곱한 값이다.
	float GetHeight(float x, float z) {
		return(m_pHeightMapImage->GetHeight(x / m_xmf3Scale.x, z / m_xmf3Scale.z) * m_xmf3Scale.y);
	}
	//지형의 법선 벡터를 계산하는 함수이다(월드 좌표계). 높이 맵의 법선 벡터를 사용한다.
	XMFLOAT3 GetNormal(float x, float z) {
		return(m_pHeightMapImage->GetHeightMapNormal(int(x / m_xmf3Scale.x), int(z /
			m_xmf3Scale.z)));
	}
	int GetHeightMapWidth() { return(m_pHeightMapImage->GetHeightMapWidth()); }
	int GetHeightMapLength() { return(m_pHeightMapImage->GetHeightMapLength()); }
	XMFLOAT3 GetScale() { return(m_xmf3Scale); }
	//지형의 크기(가로/세로)를 반환한다. 높이 맵의 크기에 스케일을 곱한 값이다.
	float GetWidth() { return(m_nWidth * m_xmf3Scale.x); }
	float GetLength() { return(m_nLength * m_xmf3Scale.z); }
};
