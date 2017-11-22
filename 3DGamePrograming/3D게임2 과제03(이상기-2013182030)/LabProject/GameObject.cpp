#include "stdafx.h"
#include "GameObject.h"

#include "Shader.h"


CTexture::CTexture(int nTextures, UINT nTextureType, int nSamplers)
{
	m_nTextureType = nTextureType;
	m_nTextures = nTextures;
	if (m_nTextures > 0)
	{
		m_pRootArgumentInfos = new SRVROOTARGUMENTINFO[m_nTextures];
		m_ppd3dTextureUploadBuffers = new ID3D12Resource*[m_nTextures];
		m_ppd3dTextures = new ID3D12Resource*[m_nTextures];
	}

	m_nSamplers = nSamplers;
	if (m_nSamplers > 0) 
		m_pd3dSamplerGpuDescriptorHandles = 
			new D3D12_GPU_DESCRIPTOR_HANDLE[m_nSamplers];
}

CTexture::~CTexture()
{
	if (m_ppd3dTextures)
	{
		for (int i = 0; i < m_nTextures; i++) if (m_ppd3dTextures[i]) m_ppd3dTextures[i]->Release();
	}

	if (m_pRootArgumentInfos)
	{
		delete[] m_pRootArgumentInfos;
	}

	if (m_pd3dSamplerGpuDescriptorHandles) delete[] m_pd3dSamplerGpuDescriptorHandles;
}

void CTexture::SetRootArgument(int nIndex, UINT nRootParameterIndex, D3D12_GPU_DESCRIPTOR_HANDLE d3dSrvGpuDescriptorHandle)
{
	m_pRootArgumentInfos[nIndex].m_nRootParameterIndex = nRootParameterIndex;
	m_pRootArgumentInfos[nIndex].m_d3dSrvGpuDescriptorHandle = d3dSrvGpuDescriptorHandle;
}

void CTexture::SetSampler(int nIndex, D3D12_GPU_DESCRIPTOR_HANDLE d3dSamplerGpuDescriptorHandle)
{
	m_pd3dSamplerGpuDescriptorHandles[nIndex] = d3dSamplerGpuDescriptorHandle;
}

void CTexture::UpdateShaderVariables(ID3D12GraphicsCommandList *pd3dCommandList)
{
	if (m_nTextureType == RESOURCE_TEXTURE2D_ARRAY)
	{
		pd3dCommandList->SetGraphicsRootDescriptorTable(m_pRootArgumentInfos[0].m_nRootParameterIndex, m_pRootArgumentInfos[0].m_d3dSrvGpuDescriptorHandle);
	}
	else
	{
		for (int i = 0; i < m_nTextures; i++)
		{
			pd3dCommandList->SetGraphicsRootDescriptorTable(m_pRootArgumentInfos[i].m_nRootParameterIndex, m_pRootArgumentInfos[i].m_d3dSrvGpuDescriptorHandle);
		}
	}
}

void CTexture::UpdateShaderVariable(ID3D12GraphicsCommandList *pd3dCommandList, int nIndex)
{
	pd3dCommandList->SetGraphicsRootDescriptorTable(
		m_pRootArgumentInfos[nIndex].m_nRootParameterIndex
		, m_pRootArgumentInfos[nIndex].m_d3dSrvGpuDescriptorHandle);
}

void CTexture::ReleaseShaderVariables()
{
}

void CTexture::ReleaseUploadBuffers()
{
	if (m_ppd3dTextureUploadBuffers)
	{
		for (int i = 0; i < m_nTextures; i++) 
			if (m_ppd3dTextureUploadBuffers[i]) 
				m_ppd3dTextureUploadBuffers[i]->Release();
		delete[] m_ppd3dTextureUploadBuffers;
		m_ppd3dTextureUploadBuffers = NULL;
	}
}

void CTexture::LoadTextureFromFile(CD3DDeviceIndRes *pd3dDeviceIndRes, ID3D12GraphicsCommandList *pd3dCommandList, wchar_t *pszFileName, UINT nIndex)
{
	m_ppd3dTextures[nIndex] = 
		pd3dDeviceIndRes->CreateTextureResourceFromFile(
			pd3dCommandList
			, pszFileName
			, &m_ppd3dTextureUploadBuffers[nIndex]
			, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
}
/////////////////////////////////////////////////////////////////////////////
CMaterial::CMaterial()
{
	m_xmf4Albedo = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
}
CMaterial::~CMaterial()
{
	if (m_pShader)
	{
		m_pShader->ReleaseShaderVariables();
		m_pShader->Release();
	}
}

void CMaterial::SetTexture(CTexture *pTexture)
{
	if (m_pTexture) m_pTexture->Release();
	m_pTexture = pTexture;
	if (m_pTexture) m_pTexture->AddRef();
}
void CMaterial::SetShader(CShader *pShader)
{
	if (m_pShader) m_pShader->Release();
	m_pShader = pShader;
	if (m_pShader) m_pShader->AddRef();
}

void CMaterial::UpdateShaderVariables(ID3D12GraphicsCommandList *pd3dCommandList)
{
	if (m_pTexture) 
		m_pTexture->UpdateShaderVariables(pd3dCommandList);
}
void CMaterial::ReleaseShaderVariables()
{
	if (m_pShader) m_pShader->ReleaseShaderVariables();
	if (m_pTexture) m_pTexture->ReleaseShaderVariables();
}
void CMaterial::ReleaseUploadBuffers()
{
	if (m_pTexture) m_pTexture->ReleaseUploadBuffers();
}
//////////////////////////////////////////////////////////////////////////
CGameObject::CGameObject(int nMeshes)
{
	m_xmf4x4World = Matrix4x4::Identity();
	m_nMeshes = nMeshes;
	m_ppMeshes = NULL;
	m_bActive = true;
	if (m_nMeshes > 0)
	{
		m_ppMeshes = new CMesh*[m_nMeshes];
		for (int i = 0; i < m_nMeshes; i++) m_ppMeshes[i] = NULL;
	}
}
CGameObject::~CGameObject()
{
	if (m_ppMeshes)
	{
		for (int i = 0; i < m_nMeshes; i++)
		{
			if (m_ppMeshes[i]) m_ppMeshes[i]->Release();
			m_ppMeshes[i] = NULL;
		}
		delete[] m_ppMeshes;
	}
	if (m_pMaterial) m_pMaterial->Release();
}

void CGameObject::SetMesh(int nIndex, CMesh *pMesh)
{
	if (m_ppMeshes)
	{
		if (m_ppMeshes[nIndex]) m_ppMeshes[nIndex]->Release();
		m_ppMeshes[nIndex] = pMesh;
		if (pMesh) pMesh->AddRef();
	}
}
void CGameObject::SetShader(CShader *pShader)
{
	if (!m_pMaterial)
	{
		m_pMaterial = new CMaterial();
		m_pMaterial->AddRef();
	}
	if (m_pMaterial) m_pMaterial->SetShader(pShader);
}

void CGameObject::SetMaterial(CMaterial *pMaterial)
{
	if (m_pMaterial) m_pMaterial->Release();
	m_pMaterial = pMaterial;
	if (m_pMaterial) m_pMaterial->AddRef();
}

void CGameObject::SetMaterial(UINT nReflection)
{
	if (!m_pMaterial) m_pMaterial = new CMaterial();
	m_pMaterial->m_nReflection = nReflection;
}

void CGameObject::CreateShaderVariables(
	  CD3DDeviceIndRes *pd3dDeviceIndRes
	, ID3D12GraphicsCommandList *pd3dCommandList)
{
}
void CGameObject::ReleaseShaderVariables()
{
}
void CGameObject::UpdateShaderVariables(ID3D12GraphicsCommandList *pd3dCommandList)
{
}

void CGameObject::Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera)
{
	if (!m_bActive) return;
	//게임 객체가 카메라에 보이면 렌더링한다.
	//if (IsVisible(pCamera))
	{
		OnPrepareRender();

		if (m_pMaterial)
		{
			if (m_pMaterial->m_pShader)
			{
				m_pMaterial->m_pShader->Render(pd3dCommandList, pCamera);
				m_pMaterial->m_pShader->UpdateShaderVariable(pd3dCommandList, &m_xmf4x4World);
			}
		}
		//pd3dCommandList->SetGraphicsRootDescriptorTable(2, m_d3dCbvGPUDescriptorHandle);
		if (m_pMaterial)
		{
			if (m_pMaterial->m_pShader)
			{
				if (dynamic_cast<CInstancingShader*>(m_pMaterial->m_pShader))
					return;
			}
		}
		if (m_ppMeshes)
			for (int i = 0; i < m_nMeshes; i++)
				if (m_ppMeshes[i])
					m_ppMeshes[i]->Render(pd3dCommandList);
	}
}
void CGameObject::Render(ID3D12GraphicsCommandList * pd3dCommandList, CCamera * pCamera, UINT nInstances)
{
	//if (!m_bActive) return;
	OnPrepareRender();
	if (m_ppMeshes)
	{
		for (int i = 0; i < m_nMeshes; i++)
			if (m_ppMeshes[i]) 
				m_ppMeshes[i]->Render(pd3dCommandList, nInstances);
	}
}
//인스턴싱 정점 버퍼 뷰를 사용하여 메쉬를 렌더링한다.
void CGameObject::Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera,
	UINT nInstances, D3D12_VERTEX_BUFFER_VIEW d3dInstancingBufferView)
{
	if (!m_bActive) return;
	OnPrepareRender();
	if (m_ppMeshes)
	{
		for (int i = 0; i < m_nMeshes; i++)
		{
			if (m_ppMeshes[i])
				m_ppMeshes[i]->Render(pd3dCommandList, nInstances, d3dInstancingBufferView);
		}
	}
}

void CGameObject::Animate(float fTimeElapsed)
{
	if (!m_bActive) return;
	if (!m_bUpdatedWorldMtx) return;
	m_xmOOBBTransformed.Transform(m_xmOOBB, XMLoadFloat4x4(&m_xmf4x4World));
	XMStoreFloat4(&m_xmOOBBTransformed.Orientation, XMQuaternionNormalize(XMLoadFloat4(&m_xmOOBBTransformed.Orientation)));
	m_bUpdatedWorldMtx = false;
}

void CGameObject::Rotate(XMFLOAT3 *pxmf3Axis, float fAngle)
{
	if (!m_bActive) return;
	XMMATRIX mtxRotate = XMMatrixRotationAxis(XMLoadFloat3(pxmf3Axis),
		XMConvertToRadians(fAngle));
	m_xmf4x4World = Matrix4x4::Multiply(mtxRotate, m_xmf4x4World);
	m_bUpdatedWorldMtx = true;
}

XMFLOAT4X4 CGameObject::GetWorldMtx()
{
	return XMFLOAT4X4(m_xmf4x4World);
}
XMFLOAT3 CGameObject::GetPosition()
{
	return(XMFLOAT3(m_xmf4x4World._41, m_xmf4x4World._42, m_xmf4x4World._43));
}
//게임 객체의 로컬 z-축 벡터를 반환한다.
XMFLOAT3 CGameObject::GetLook()
{
	return(Vector3::Normalize(
		XMFLOAT3(m_xmf4x4World._31, m_xmf4x4World._32, m_xmf4x4World._33)));
}
//게임 객체의 로컬 y-축 벡터를 반환한다.
XMFLOAT3 CGameObject::GetUp()
{
	return(Vector3::Normalize(
		XMFLOAT3(m_xmf4x4World._21, m_xmf4x4World._22, m_xmf4x4World._23)));
}
//게임 객체의 로컬 x-축 벡터를 반환한다.
XMFLOAT3 CGameObject::GetRight()
{
	return(Vector3::Normalize(
		XMFLOAT3(m_xmf4x4World._11, m_xmf4x4World._12, m_xmf4x4World._13)));
}

void CGameObject::SetPosition(float x, float y, float z)
{
	if (!m_bActive) return;
	m_xmf4x4World._41 = x;
	m_xmf4x4World._42 = y;
	m_xmf4x4World._43 = z;
	m_bUpdatedWorldMtx = true;
}
void CGameObject::SetPosition(XMFLOAT3 xmf3Position)
{
	SetPosition(xmf3Position.x, xmf3Position.y, xmf3Position.z);
}

//게임 객체를 로컬 x-축 방향으로 이동한다.
void CGameObject::MoveStrafe(float fDistance)
{
	XMFLOAT3 xmf3Position = GetPosition();
	XMFLOAT3 xmf3Right = GetRight();
	xmf3Position = Vector3::Add(xmf3Position, xmf3Right, fDistance);
	CGameObject::SetPosition(xmf3Position);
}
//게임 객체를 로컬 y-축 방향으로 이동한다.
void CGameObject::MoveUp(float fDistance)
{
	XMFLOAT3 xmf3Position = GetPosition();
	XMFLOAT3 xmf3Up = GetUp();
	xmf3Position = Vector3::Add(xmf3Position, xmf3Up, fDistance);
	CGameObject::SetPosition(xmf3Position);
}
//게임 객체를 로컬 z-축 방향으로 이동한다.
void CGameObject::MoveForward(float fDistance)
{
	XMFLOAT3 xmf3Position = GetPosition();
	XMFLOAT3 xmf3Look = GetLook();
	xmf3Position = Vector3::Add(xmf3Position, xmf3Look, fDistance);
	CGameObject::SetPosition(xmf3Position);
}
void CGameObject::Move(XMFLOAT3 & vDirection, float fSpeed)
{
	SetPosition(
		m_xmf4x4World._41 + vDirection.x * fSpeed,
		m_xmf4x4World._42 + vDirection.y * fSpeed,
		m_xmf4x4World._43 + vDirection.z * fSpeed);
}
//게임 객체를 주어진 각도로 회전한다.
void CGameObject::Rotate(float fPitch, float fYaw, float fRoll)
{
	if (!m_bActive) return;
	XMMATRIX mtxRotate = XMMatrixRotationRollPitchYaw(
		XMConvertToRadians(fPitch),
		XMConvertToRadians(fYaw), 
		XMConvertToRadians(fRoll));
	m_xmf4x4World = Matrix4x4::Multiply(mtxRotate, m_xmf4x4World);
	m_bUpdatedWorldMtx = true;
}

void CGameObject::ReleaseUploadBuffers()
{
	if (m_ppMeshes)
	{
		for (int i = 0; i < m_nMeshes; i++)
		{
			if (m_ppMeshes[i]) m_ppMeshes[i]->ReleaseUploadBuffers();
		}
	}
}

bool CGameObject::IsVisible(CCamera *pCamera)
{
	OnPrepareRender();
	bool bIsVisible = false;
	if (m_ppMeshes)
	{
		for (int i = 0; i < m_nMeshes; i++)
		{
			switch (m_Tag)
			{
			case ObjectType::Background:
				return true;
			case -1:
			{
				BoundingOrientedBox xmBoundingBox = m_ppMeshes[i]->GetBoundingBox();
				//모델 좌표계의 바운딩 박스를 월드 좌표계로 변환한다.
				xmBoundingBox.Transform(xmBoundingBox, XMLoadFloat4x4(&m_xmf4x4World));
				if (pCamera)
					return pCamera->IsInFrustum(xmBoundingBox);
				break;
				}
			default:
				if (pCamera)
					return pCamera->IsInFrustum(m_xmOOBB);
				break;
			}
		
		}
	}
	return(bIsVisible);
}

void CGameObject::GenerateRayForPicking(XMFLOAT3& xmf3PickPosition, XMFLOAT4X4&
	xmf4x4View, XMFLOAT3 *pxmf3PickRayOrigin, XMFLOAT3 *pxmf3PickRayDirection)
{
	XMFLOAT4X4 xmf4x4WorldView = Matrix4x4::Multiply(m_xmf4x4World, xmf4x4View);
	XMFLOAT4X4 xmf4x4Inverse = Matrix4x4::Inverse(xmf4x4WorldView);
	XMFLOAT3 xmf3CameraOrigin(0.0f, 0.0f, 0.0f);
	//카메라 좌표계의 원점을 모델 좌표계로 변환한다.
	*pxmf3PickRayOrigin = Vector3::TransformCoord(xmf3CameraOrigin, xmf4x4Inverse);
	//카메라 좌표계의 점(마우스 좌표를 역변환하여 구한 점)을 모델 좌표계로 변환한다.
	*pxmf3PickRayDirection = Vector3::TransformCoord(xmf3PickPosition, xmf4x4Inverse);
	//광선의 방향 벡터를 구한다.
	*pxmf3PickRayDirection = Vector3::Normalize(Vector3::Subtract(*pxmf3PickRayDirection,
		*pxmf3PickRayOrigin));
}

int CGameObject::PickObjectByRayIntersection(XMFLOAT3& xmf3PickPosition, XMFLOAT4X4&
	xmf4x4View, float *pfHitDistance)
{
	int nIntersected = 0;
	if (m_ppMeshes)
	{
		XMFLOAT3 xmf3PickRayOrigin, xmf3PickRayDirection;
		//모델 좌표계의 광선을 생성한다.
		GenerateRayForPicking(xmf3PickPosition, xmf4x4View, &xmf3PickRayOrigin,
			&xmf3PickRayDirection);
		for (int i = 0; i < m_nMeshes; i++)
		{
			//모델 좌표계의 광선과 메쉬의 교차를 검사한다.
			nIntersected = m_ppMeshes[i]->CheckRayIntersection(xmf3PickRayOrigin,
				xmf3PickRayDirection, pfHitDistance);
			return(nIntersected);
		}
	}
	return(nIntersected);
}

CRotatingObject::CRotatingObject(int nMeshes) : CGameObject(nMeshes)
{
	m_xmf3RotationAxis = XMFLOAT3(0.0f, 1.0f, 0.0f);
	m_fRotationSpeed = 15.0f;
}
CRotatingObject::~CRotatingObject()
{
}

void CRotatingObject::Animate(float fTimeElapsed)
{
	if (!CGameObject::IsActive()) return;

	if (m_fRotationSpeed != 0.0f)
		CGameObject::Rotate(&m_xmf3RotationAxis, m_fRotationSpeed * fTimeElapsed);
	if (m_fMovingSpeed != 0.0f)
		CGameObject::Move(m_xmf3MovingDirection, m_fMovingSpeed * fTimeElapsed);

	if (!m_bUpdatedWorldMtx) return;
	m_xmOOBBTransformed.Transform(m_xmOOBB, XMLoadFloat4x4(&m_xmf4x4World));
	XMStoreFloat4(&m_xmOOBBTransformed.Orientation, 
		XMQuaternionNormalize(XMLoadFloat4(
			&m_xmOOBBTransformed.Orientation)));
	m_bUpdatedWorldMtx = false;
}

void CRotatingObject::SetMovingDirection(XMFLOAT3 & xmf3MovingDirection)
{
	m_xmf3MovingDirection = Vector3::Normalize(xmf3MovingDirection);
	m_xmf3RotationAxis = Vector3::CrossProduct(m_xmf3MovingDirection, XMFLOAT3(0, 1, 0));
}

int n = 0;
CRevolvingObject::CRevolvingObject()
	:CGameObject(1)
{
	m_xmf3RevolutionAxis = XMFLOAT3(n, 0.0f, 1.0f);
	if (n++ == 0)
		m_fRevolutionSpeed = 0.0f;
	else
		m_fRevolutionSpeed = 3.14f;
}

CRevolvingObject::~CRevolvingObject()
{
}

void CRevolvingObject::SetDay()
{
	if (m_xmf3RevolutionAxis.x == 0)
		m_fRevolutionSpeed = 0.0f;
	else if(m_xmf3RevolutionAxis.x == 1)
		m_fRevolutionSpeed = 3.14f;
}

void CRevolvingObject::SetNight()
{
	if (m_xmf3RevolutionAxis.x == 0)
		m_fRevolutionSpeed = 3.14f;
	else if (m_xmf3RevolutionAxis.x == 1)
		m_fRevolutionSpeed = 0.0f;
}

void CRevolvingObject::Animate(float fTimeElapsed)
{
	m_fRevolutionSpeed += fTimeElapsed * 0.3f;
	m_xmf4x4World._41 = 1400 * cos(m_fRevolutionSpeed) + 800;
	m_xmf4x4World._42 = 1400 * sin(m_fRevolutionSpeed);
	m_xmf4x4World._43 = 800;
	if (m_fRevolutionSpeed > 6.28) m_fRevolutionSpeed -= 6.28;
	//XMMATRIX mtxRotate = XMMatrixRotationAxis(
	//	XMLoadFloat3(&m_xmf3RevolutionAxis), 
	//	XMConvertToRadians(m_fRevolutionSpeed * fTimeElapsed));
	//m_xmf4x4World = Matrix4x4::Multiply(m_xmf4x4World, mtxRotate);
}

CHeightMapTerrain::CHeightMapTerrain(
	  CD3DDeviceIndRes *pd3dDeviceIndRes
	, ID3D12GraphicsCommandList *pd3dCommandList
	, ID3D12RootSignature *pd3dGraphicsRootSignature
	, LPCTSTR pFileName, int nWidth, int nLength
	, int nBlockWidth, int nBlockLength
	, XMFLOAT3 xmf3Scale, XMFLOAT4 xmf4Color) 
	: CGameObject(0)
{
	m_Tag = ObjectType::Background;
	m_nWidth = nWidth;
	m_nLength = nLength;

	int cxQuadsPerBlock = nBlockWidth - 1;
	int czQuadsPerBlock = nBlockLength - 1;

	m_xmf3Scale = xmf3Scale;
	m_pHeightMapImage = new CHeightMapImage(
		pFileName, nWidth, nLength, xmf3Scale);

	long cxBlocks = (m_nWidth - 1) / cxQuadsPerBlock;
	long czBlocks = (m_nLength - 1) / czQuadsPerBlock;

	m_nMeshes = cxBlocks * czBlocks;
	m_ppMeshes = new CMesh*[m_nMeshes];

	for (int i = 0; i < m_nMeshes; i++) 
		m_ppMeshes[i] = NULL;

	CHeightMapGridMeshTextured *pHeightMapGridMesh = NULL;
	for (int z = 0, zStart = 0; z < czBlocks; z++)
	{
		for (int x = 0, xStart = 0; x < cxBlocks; x++)
		{
			xStart = x * (nBlockWidth - 1);
			zStart = z * (nBlockLength - 1);
			pHeightMapGridMesh = new CHeightMapGridMeshTextured(
				pd3dDeviceIndRes
				, pd3dCommandList
				, xStart, zStart
				, nBlockWidth, nBlockLength
				, xmf3Scale, xmf4Color
				, m_pHeightMapImage);
			SetMesh(x + (z*cxBlocks), pHeightMapGridMesh);
		}
	}
	CTexture *pTerrainTexture = new CTexture(2, RESOURCE_TEXTURE2D, 0);

	pTerrainTexture->LoadTextureFromFile(pd3dDeviceIndRes, pd3dCommandList, L"Assets/Image/Terrain/Base_Texture.dds", 0);
	pTerrainTexture->LoadTextureFromFile(pd3dDeviceIndRes, pd3dCommandList, L"Assets/Image/Terrain/Detail_Texture_7.dds", 1);

	CMaterial *pTerrainMaterial = new CMaterial();
	pTerrainMaterial->SetTexture(pTerrainTexture);
	SetMaterial(pTerrainMaterial);

	CTexturedTerrainShader *pShader = new CTexturedTerrainShader();
	pShader->CreateShader(pd3dDeviceIndRes, pd3dGraphicsRootSignature);
	pShader->SetTerrain(this);
	pShader->CreateShaderVariables(pd3dDeviceIndRes, pd3dCommandList);
	pShader->CreateCbvAndSrvDescriptorHeaps(pd3dDeviceIndRes, pd3dCommandList, 0, 2);
	pShader->CreateShaderResourceViews(pd3dDeviceIndRes, pd3dCommandList, pTerrainTexture, 5, false);
	pTerrainMaterial->SetShader(pShader);
}
CHeightMapTerrain::~CHeightMapTerrain(void)
{
	if (m_pHeightMapImage) delete m_pHeightMapImage;
}

void CHeightMapTerrain::Render(ID3D12GraphicsCommandList * pd3dCommandList, CCamera * pCamera)
{
	OnPrepareRender();

	if (m_pMaterial)
	{
		if (m_pMaterial->m_pShader)
		{
			m_pMaterial->m_pShader->Render(pd3dCommandList, pCamera);
			m_pMaterial->m_pShader->UpdateShaderVariable(pd3dCommandList, &m_xmf4x4World);
		}
	}

	if (m_ppMeshes)
	{
		for (int i = 0; i < m_nMeshes; i++)
		{
			if (m_pMaterial)
			{
				if (m_pMaterial->m_pTexture)
					m_pMaterial->m_pTexture->UpdateShaderVariable(pd3dCommandList, i);
			}
			if (m_ppMeshes[i]) m_ppMeshes[i]->Render(pd3dCommandList);
		}
	}
}

CSkyBox::CSkyBox(
	CD3DDeviceIndRes *pd3dDeviceIndRes
	, ID3D12GraphicsCommandList *pd3dCommandList
	, ID3D12RootSignature *pd3dGraphicsRootSignature) 
	: CGameObject(6)
{
	CRectMeshTextured *pSkyBoxMesh = new CRectMeshTextured(pd3dDeviceIndRes, pd3dCommandList, 20.0f, 20.0f, 0.0f, 0.0f, 0.0f, +10.0f);
	SetMesh(0, pSkyBoxMesh);
	pSkyBoxMesh = new CRectMeshTextured(pd3dDeviceIndRes, pd3dCommandList, 20.0f, 20.0f, 0.0f, 0.0f, 0.0f, -10.0f);
	SetMesh(1, pSkyBoxMesh);
	pSkyBoxMesh = new CRectMeshTextured(pd3dDeviceIndRes, pd3dCommandList, 0.0f, 20.0f, 20.0f, -10.0f, 0.0f, 0.0f);
	SetMesh(2, pSkyBoxMesh);
	pSkyBoxMesh = new CRectMeshTextured(pd3dDeviceIndRes, pd3dCommandList, 0.0f, 20.0f, 20.0f, +10.0f, 0.0f, 0.0f);
	SetMesh(3, pSkyBoxMesh);
	pSkyBoxMesh = new CRectMeshTextured(pd3dDeviceIndRes, pd3dCommandList, 20.0f, 0.0f, 20.0f, 0.0f, +10.0f, 0.0f);
	SetMesh(4, pSkyBoxMesh);
	pSkyBoxMesh = new CRectMeshTextured(pd3dDeviceIndRes, pd3dCommandList, 20.0f, 0.0f, 20.0f, 0.0f, -10.0f, 0.0f);
	SetMesh(5, pSkyBoxMesh);

	CTexture *pSkyBoxTexture = new CTexture(6, RESOURCE_TEXTURE2D, 0);
	pSkyBoxTexture->LoadTextureFromFile(pd3dDeviceIndRes, pd3dCommandList, L"Assets/Image/SkyBox/SkyBox_Front_0.dds", 0);
	pSkyBoxTexture->LoadTextureFromFile(pd3dDeviceIndRes, pd3dCommandList, L"Assets/Image/SkyBox/SkyBox_Back_0.dds", 1);
	pSkyBoxTexture->LoadTextureFromFile(pd3dDeviceIndRes, pd3dCommandList, L"Assets/Image/SkyBox/SkyBox_Left_0.dds", 2);
	pSkyBoxTexture->LoadTextureFromFile(pd3dDeviceIndRes, pd3dCommandList, L"Assets/Image/SkyBox/SkyBox_Right_0.dds", 3);
	pSkyBoxTexture->LoadTextureFromFile(pd3dDeviceIndRes, pd3dCommandList, L"Assets/Image/SkyBox/SkyBox_Top_0.dds", 4);
	pSkyBoxTexture->LoadTextureFromFile(pd3dDeviceIndRes, pd3dCommandList, L"Assets/Image/SkyBox/SkyBox_Bottom_0.dds", 5);

	CMaterial *pSkyBoxMaterial = new CMaterial();
	pSkyBoxMaterial->SetTexture(pSkyBoxTexture);

	SetMaterial(pSkyBoxMaterial);
	CSkyBoxShader *pSkyBoxShader = new CSkyBoxShader();
	pSkyBoxShader->CreateShader(pd3dDeviceIndRes, pd3dGraphicsRootSignature);
	pSkyBoxShader->SetSkyBox(this);
	pSkyBoxShader->CreateShaderVariables(pd3dDeviceIndRes, pd3dCommandList);
	pSkyBoxShader->CreateCbvAndSrvDescriptorHeaps(pd3dDeviceIndRes, pd3dCommandList, 0, 6);
	pSkyBoxShader->CreateShaderResourceViews(pd3dDeviceIndRes, pd3dCommandList, pSkyBoxTexture, 5, false);
	pSkyBoxMaterial->SetShader(pSkyBoxShader);
}

CSkyBox::~CSkyBox()
{
}

void CSkyBox::Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera)
{
	XMFLOAT3 xmf3CameraPos = pCamera->GetPosition();
	SetPosition(xmf3CameraPos.x, xmf3CameraPos.y, xmf3CameraPos.z);

	OnPrepareRender();

	if (m_pMaterial)
	{
		if (m_pMaterial->m_pShader)
		{
			m_pMaterial->m_pShader->Render(pd3dCommandList, pCamera);
			m_pMaterial->m_pShader->UpdateShaderVariable(pd3dCommandList, &m_xmf4x4World);
		}
	}

	if (m_ppMeshes)
	{
		for (int i = 0; i < m_nMeshes; i++)
		{
			if (m_pMaterial)
			{
				if (m_pMaterial->m_pTexture)
					m_pMaterial->m_pTexture->UpdateShaderVariable(pd3dCommandList, i);
			}
			if (m_ppMeshes[i]) m_ppMeshes[i]->Render(pd3dCommandList);
		}
	}
}
