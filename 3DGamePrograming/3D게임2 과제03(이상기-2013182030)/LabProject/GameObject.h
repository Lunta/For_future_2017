#pragma once
#include "Mesh.h"
#include "Camera.h"

class CShader;
struct CB_GAMEOBJECT_INFO
{
	XMFLOAT4X4						m_xmf4x4World;
	UINT							m_nMaterial;
};
struct SRVROOTARGUMENTINFO
{
	UINT							m_nRootParameterIndex = 0;
	D3D12_GPU_DESCRIPTOR_HANDLE		m_d3dSrvGpuDescriptorHandle;
};

class CTexture
{
public:
	CTexture(int nTextureResources = 1, UINT nResourceType = RESOURCE_TEXTURE2D, int nSamplers = 0);
	virtual ~CTexture();

private:
	int								m_nReferences = 0;

	UINT							m_nTextureType = RESOURCE_TEXTURE2D;
	int								m_nTextures = 0;
	ID3D12Resource					**m_ppd3dTextures = NULL;
	ID3D12Resource					**m_ppd3dTextureUploadBuffers;
	SRVROOTARGUMENTINFO				*m_pRootArgumentInfos = NULL;

	int								m_nSamplers = 0;
	D3D12_GPU_DESCRIPTOR_HANDLE		*m_pd3dSamplerGpuDescriptorHandles = NULL;

public:
	void AddRef() { m_nReferences++; }
	void Release() { if (--m_nReferences <= 0) delete this; }

	void SetRootArgument(int nIndex, UINT nRootParameterIndex, D3D12_GPU_DESCRIPTOR_HANDLE d3dsrvGpuDescriptorHandle);
	void SetSampler(int nIndex, D3D12_GPU_DESCRIPTOR_HANDLE d3dSamplerGpuDescriptorHandle);

	void UpdateShaderVariables(ID3D12GraphicsCommandList *pd3dCommandList);
	void UpdateShaderVariable(ID3D12GraphicsCommandList *pd3dCommandList, int nIndex);
	void ReleaseShaderVariables();

	void LoadTextureFromFile(CD3DDeviceIndRes* pd3dDeviceIndRes, ID3D12GraphicsCommandList *pd3dCommandList, wchar_t *pszFileName, UINT nIndex);

	int GetTextureCount() { return(m_nTextures); }
	ID3D12Resource *GetTexture(int nIndex) { return(m_ppd3dTextures[nIndex]); }
	UINT GetTextureType() { return(m_nTextureType); }

	void ReleaseUploadBuffers();
};

class CMaterial
{
public:
	CMaterial();
	virtual ~CMaterial();

private:
	int								m_nReferences = 0;

public:
	void AddRef() { m_nReferences++; }
	void Release() { if (--m_nReferences <= 0) delete this; }

	XMFLOAT4						m_xmf4Albedo = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

	UINT							m_nReflection = 0;
	CTexture						*m_pTexture = NULL;
	CShader							*m_pShader = NULL;

	void SetAlbedo(XMFLOAT4 xmf4Albedo) { m_xmf4Albedo = xmf4Albedo; }
	void SetReflection(UINT nReflection) { m_nReflection = nReflection; };
	void SetTexture(CTexture *pTexture);
	void SetShader(CShader *pShader);

	void UpdateShaderVariables(ID3D12GraphicsCommandList *pd3dCommandList);
	void ReleaseShaderVariables();

	void ReleaseUploadBuffers();
};

class CGameObject
{
public:
	CGameObject(int nMeshes = 1);
	virtual ~CGameObject();

public:
	TCHAR							m_pstrFrameName[256];

	XMFLOAT4X4						m_xmf4x4Transform;
	XMFLOAT4X4						m_xmf4x4World;

	CMesh							**m_ppMeshes;
	int								m_nMeshes;

	CMaterial						*m_pMaterial = NULL;

	D3D12_GPU_DESCRIPTOR_HANDLE		m_d3dCbvGPUDescriptorHandle;

protected:
	ComPtr<ID3D12Resource>			m_pd3dcbGameObject = NULL;
	CB_GAMEOBJECT_INFO				*m_pcbMappedGameObject = NULL;

public:
	void SetMesh(int nIndex, CMesh *pMesh);
	void SetShader(CShader *pShader);
	void SetMaterial(CMaterial *pMaterial);

	void ResizeMeshes(int nMeshes);

	void SetCbvGPUDescriptorHandle(D3D12_GPU_DESCRIPTOR_HANDLE d3dCbvGPUDescriptorHandle) { m_d3dCbvGPUDescriptorHandle = d3dCbvGPUDescriptorHandle; }
	void SetCbvGPUDescriptorHandlePtr(UINT64 nCbvGPUDescriptorHandlePtr) { m_d3dCbvGPUDescriptorHandle.ptr = nCbvGPUDescriptorHandlePtr; }
	D3D12_GPU_DESCRIPTOR_HANDLE GetCbvGPUDescriptorHandle() { return(m_d3dCbvGPUDescriptorHandle); }

	virtual void CreateShaderVariables(CD3DDeviceIndRes* pd3dDeviceIndRes, ID3D12GraphicsCommandList *pd3dCommandList);
	virtual void ReleaseShaderVariables();
	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList *pd3dCommandList);

	virtual void Animate(float fTimeElapsed);
	virtual void OnPrepareRender() { }
	virtual void SetRootParameter(ID3D12GraphicsCommandList *pd3dCommandList);
	virtual void Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera = NULL);

	virtual void BuildMaterials(CD3DDeviceIndRes* pd3dDeviceIndRes, ID3D12GraphicsCommandList *pd3dCommandList) { }
	virtual void ReleaseUploadBuffers();

	XMFLOAT3 GetPosition();
	XMFLOAT3 GetLook();
	XMFLOAT3 GetUp();
	XMFLOAT3 GetRight();

	void SetPosition(float x, float y, float z);
	void SetPosition(XMFLOAT3 xmf3Position);
	void SetLocalPosition(XMFLOAT3 xmf3Position);
	void SetScale(float x, float y, float z);
	void SetLocalScale(float x, float y, float z);

	void MoveStrafe(float fDistance = 1.0f);
	void MoveUp(float fDistance = 1.0f);
	void MoveForward(float fDistance = 1.0f);

	void Rotate(float fPitch = 10.0f, float fYaw = 10.0f, float fRoll = 10.0f);
	void Rotate(XMFLOAT3 *pxmf3Axis, float fAngle);
	void Rotate(XMFLOAT4 *pxmf4Quaternion);

	void UpdateTransform(XMFLOAT4X4 *pxmf4x4Parent = NULL);

public:
	CGameObject*						m_pParent = NULL;
	CGameObject*						m_pChild = NULL;
	CGameObject*						m_pSibling = NULL;

	void			SetChild(CGameObject *pChild);
	CGameObject*	GetParent()							{ return(m_pParent); }
	CGameObject*	FindFrame(_TCHAR *pstrFrameName);

	void LoadFrameHierarchyFromFile(
		  CD3DDeviceIndRes*				pd3dDeviceIndRes
		, ID3D12GraphicsCommandList*	pd3dCommandList
		, ID3D12RootSignature*			pd3dGraphicsRootSignature
		, wifstream&					InFile
		, UINT							nFrame);
	void LoadGeometryFromFile(
		  CD3DDeviceIndRes*				pd3dDeviceIndRes
		, ID3D12GraphicsCommandList*	pd3dCommandList
		, ID3D12RootSignature*			pd3dGraphicsRootSignature
		, TCHAR*						pstrFileName);
	void PrintFrameInfo(CGameObject* pGameObject, CGameObject *pParent);
};

class CApacheHellicopter : public CGameObject
{
public:
	CApacheHellicopter(
		  CD3DDeviceIndRes*					pd3dDeviceIndRes
		, ID3D12GraphicsCommandList*		pd3dCommandList
		, ID3D12RootSignature*				pd3dGraphicsRootSignature);
	virtual ~CApacheHellicopter();

	virtual void Animate(float fTimeElapsed);

	CGameObject					*m_pRotorFrame = NULL;
};

class CFlyerShip : public CGameObject
{
public:
	CFlyerShip(
		  CD3DDeviceIndRes*					pd3dDeviceIndRes
		, ID3D12GraphicsCommandList*		pd3dCommandList
		, ID3D12RootSignature*				pd3dGraphicsRootSignature);
	virtual ~CFlyerShip();
};

class CHeightMapTerrain : public CGameObject
{
public:
	CHeightMapTerrain(
		  CD3DDeviceIndRes*				pd3dDeviceIndRes
		, ID3D12GraphicsCommandList*	pd3dCommandList
		, ID3D12RootSignature*			pd3dGraphicsRootSignature
		, LPCTSTR						pFileName
		, int							nWidth
		, int							nLength
		, int							nBlockWidth
		, int							nBlockLength
		, XMFLOAT3						xmf3Scale
		, XMFLOAT4						xmf4Color);
	virtual ~CHeightMapTerrain();
private:
	CHeightMapImage*					m_pHeightMapImage;

	int									m_nWidth;
	int									m_nLength;

	XMFLOAT3							m_xmf3Scale;

public:
	float GetHeight(float x, float z) 
	{
		return(m_pHeightMapImage->GetHeight(x / m_xmf3Scale.x, z / m_xmf3Scale.z) * m_xmf3Scale.y);
	}
	XMFLOAT3 GetNormal(float x, float z) {
		return(m_pHeightMapImage->GetHeightMapNormal(int(x / m_xmf3Scale.x), int(z / m_xmf3Scale.z)));
	}
	int GetHeightMapWidth() { return(m_pHeightMapImage->GetHeightMapWidth()); }
	int GetHeightMapLength() { return(m_pHeightMapImage->GetHeightMapLength()); }
	XMFLOAT3 GetScale() { return(m_xmf3Scale); }

	float GetWidth() { return(m_nWidth * m_xmf3Scale.x); }
	float GetLength() { return(m_nLength * m_xmf3Scale.z); }

	virtual void Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera = NULL);
};

class CSkyBox : public CGameObject
{
public:
	CSkyBox(CD3DDeviceIndRes* pd3dDeviceIndRes, ID3D12GraphicsCommandList *pd3dCommandList, ID3D12RootSignature *pd3dGraphicsRootSignature);
	virtual ~CSkyBox();

	virtual void Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera = NULL);
};