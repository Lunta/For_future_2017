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
		m_pd3dSamplerGpuDescriptorHandles = new D3D12_GPU_DESCRIPTOR_HANDLE[m_nSamplers];
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

void CTexture::LoadTextureFromFile(CD3DDeviceIndRes* pd3dDeviceIndRes, ID3D12GraphicsCommandList *pd3dCommandList, wchar_t *pszFileName, UINT nIndex)
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
}
CMaterial::~CMaterial()
{
	if (m_pShader)
	{
		m_pShader->ReleaseShaderVariables();
		m_pShader->Release();
	}
	if (m_pTexture) m_pTexture->Release();
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
	if (m_pTexture) m_pTexture->UpdateShaderVariables(pd3dCommandList);
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
	m_xmf4x4Transform = Matrix4x4::Identity();
	m_xmf4x4World = Matrix4x4::Identity();

	m_nMeshes = nMeshes;
	m_ppMeshes = NULL;
	if (m_nMeshes > 0)
	{
		m_ppMeshes = new CMesh*[m_nMeshes];
		for (int i = 0; i < m_nMeshes; i++)	m_ppMeshes[i] = NULL;
	}
}

CGameObject::~CGameObject()
{
	ReleaseShaderVariables();

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

	if (m_pSibling) delete m_pSibling;
	if (m_pChild) delete m_pChild;
}

void CGameObject::ResizeMeshes(int nMeshes)
{
	if (m_ppMeshes)
	{
		for (int i = 0; i < m_nMeshes; i++)
		{
			if (m_ppMeshes[i]) m_ppMeshes[i]->Release();
			m_ppMeshes[i] = NULL;
		}
		delete[] m_ppMeshes;
		m_ppMeshes = NULL;
	}

	m_nMeshes = nMeshes;
	m_ppMeshes = NULL;
	if (m_nMeshes > 0)
	{
		m_ppMeshes = new CMesh*[m_nMeshes];
		for (int i = 0; i < m_nMeshes; i++)	m_ppMeshes[i] = NULL;
	}
}

void CGameObject::SetChild(CGameObject *pChild)
{
	if (m_pChild)
	{
		if (pChild) pChild->m_pSibling = m_pChild->m_pSibling;
		m_pChild->m_pSibling = pChild;
	}
	else
	{
		m_pChild = pChild;
	}
	if (pChild) pChild->m_pParent = this;
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
		CMaterial *pMaterial = new CMaterial();
		SetMaterial(pMaterial);
	}
	if (m_pMaterial) m_pMaterial->SetShader(pShader);
}

void CGameObject::SetMaterial(CMaterial *pMaterial)
{
	if (m_pMaterial) m_pMaterial->Release();
	m_pMaterial = pMaterial;
	if (m_pMaterial) m_pMaterial->AddRef();
}

void CGameObject::CreateShaderVariables(
	  CD3DDeviceIndRes* pd3dDeviceIndRes
	, ID3D12GraphicsCommandList *pd3dCommandList)
{
	UINT ncbElementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255); //256의 배수
	m_pd3dcbGameObject = pd3dDeviceIndRes->CreateBufferResource(
		pd3dCommandList
		, NULL
		, ncbElementBytes
		, D3D12_HEAP_TYPE_UPLOAD
		, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER
		, NULL);

	m_pd3dcbGameObject->Map(0, NULL, (void **)&m_pcbMappedGameObject);
}
void CGameObject::ReleaseShaderVariables()
{
	if (m_pd3dcbGameObject) m_pd3dcbGameObject->Unmap(0, NULL);
	if (m_pMaterial) m_pMaterial->ReleaseShaderVariables();
}
void CGameObject::UpdateShaderVariables(ID3D12GraphicsCommandList *pd3dCommandList)
{
	XMStoreFloat4x4(&m_pcbMappedGameObject->m_xmf4x4World, XMMatrixTranspose(XMLoadFloat4x4(&m_xmf4x4World)));
	if (m_pMaterial) m_pcbMappedGameObject->m_nMaterial = m_pMaterial->m_nReflection;
}

void CGameObject::Animate(float fTimeElapsed)
{
	if (m_pSibling) m_pSibling->Animate(fTimeElapsed);
	if (m_pChild) m_pChild->Animate(fTimeElapsed);
}

void CGameObject::SetRootParameter(ID3D12GraphicsCommandList *pd3dCommandList)
{
	pd3dCommandList->SetGraphicsRootDescriptorTable(2, m_d3dCbvGPUDescriptorHandle);
}

void CGameObject::Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera)
{
	OnPrepareRender();

	if (m_pMaterial)
	{
		if (m_pMaterial->m_pShader)
		{
			m_pMaterial->m_pShader->Render(pd3dCommandList, pCamera);
			m_pMaterial->m_pShader->UpdateShaderVariables(pd3dCommandList);

			UpdateShaderVariables(pd3dCommandList);
		}
		if (m_pMaterial->m_pTexture)
		{
			m_pMaterial->m_pTexture->UpdateShaderVariables(pd3dCommandList);
		}
	}

	if (m_ppMeshes)
	{
		SetRootParameter(pd3dCommandList);

		for (int i = 0; i < m_nMeshes; i++)
		{
			if (m_ppMeshes[i]) m_ppMeshes[i]->Render(pd3dCommandList);
		}
	}

	if (m_pSibling) m_pSibling->Render(pd3dCommandList, pCamera);
	if (m_pChild) m_pChild->Render(pd3dCommandList, pCamera);
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

	if (m_pMaterial) m_pMaterial->ReleaseUploadBuffers();

	if (m_pSibling) m_pSibling->ReleaseUploadBuffers();
	if (m_pChild) m_pChild->ReleaseUploadBuffers();
}

void CGameObject::UpdateTransform(XMFLOAT4X4 *pxmf4x4Parent)
{
	m_xmf4x4World = (pxmf4x4Parent) ? Matrix4x4::Multiply(m_xmf4x4Transform, *pxmf4x4Parent) : m_xmf4x4Transform;

	if (m_pSibling) m_pSibling->UpdateTransform(pxmf4x4Parent);
	if (m_pChild) m_pChild->UpdateTransform(&m_xmf4x4World);
}

CGameObject *CGameObject::FindFrame(_TCHAR *pstrFrameName)
{
	CGameObject *pFrameObject = NULL;
	if (!_tcsncmp(m_pstrFrameName, pstrFrameName, _tcslen(pstrFrameName))) return(this);

	if (m_pSibling) if (pFrameObject = m_pSibling->FindFrame(pstrFrameName)) return(pFrameObject);
	if (m_pChild) if (pFrameObject = m_pChild->FindFrame(pstrFrameName)) return(pFrameObject);

	return(NULL);
}

void CGameObject::SetPosition(float x, float y, float z)
{
	m_xmf4x4World._41 = x;
	m_xmf4x4World._42 = y;
	m_xmf4x4World._43 = z;

	UpdateTransform(NULL);
}

void CGameObject::SetPosition(XMFLOAT3 xmf3Position)
{
	SetPosition(xmf3Position.x, xmf3Position.y, xmf3Position.z);
}

void CGameObject::SetLocalPosition(XMFLOAT3 xmf3Position)
{
	XMMATRIX mtxTranslation = XMMatrixTranslation(xmf3Position.x, xmf3Position.y, xmf3Position.z);
	m_xmf4x4Transform = Matrix4x4::Multiply(m_xmf4x4Transform, mtxTranslation);

	UpdateTransform(NULL);
}

void CGameObject::SetScale(float x, float y, float z)
{
	XMMATRIX mtxScale = XMMatrixScaling(x, y, z);
	m_xmf4x4Transform = Matrix4x4::Multiply(mtxScale, m_xmf4x4Transform);

	UpdateTransform(NULL);
}

void CGameObject::SetLocalScale(float x, float y, float z)
{
}

XMFLOAT3 CGameObject::GetPosition()
{
	return(XMFLOAT3(m_xmf4x4World._41, m_xmf4x4World._42, m_xmf4x4World._43));
}

XMFLOAT3 CGameObject::GetLook()
{
	return(Vector3::Normalize(XMFLOAT3(m_xmf4x4World._31, m_xmf4x4World._32, m_xmf4x4World._33)));
}

XMFLOAT3 CGameObject::GetUp()
{
	return(Vector3::Normalize(XMFLOAT3(m_xmf4x4World._21, m_xmf4x4World._22, m_xmf4x4World._23)));
}

XMFLOAT3 CGameObject::GetRight()
{
	return(Vector3::Normalize(XMFLOAT3(m_xmf4x4World._11, m_xmf4x4World._12, m_xmf4x4World._13)));
}

void CGameObject::MoveStrafe(float fDistance)
{
	XMFLOAT3 xmf3Position = GetPosition();
	XMFLOAT3 xmf3Right = GetRight();
	xmf3Position = Vector3::Add(xmf3Position, xmf3Right, fDistance);
	CGameObject::SetPosition(xmf3Position);
}

void CGameObject::MoveUp(float fDistance)
{
	XMFLOAT3 xmf3Position = GetPosition();
	XMFLOAT3 xmf3Up = GetUp();
	xmf3Position = Vector3::Add(xmf3Position, xmf3Up, fDistance);
	CGameObject::SetPosition(xmf3Position);
}

void CGameObject::MoveForward(float fDistance)
{
	XMFLOAT3 xmf3Position = GetPosition();
	XMFLOAT3 xmf3Look = GetLook();
	xmf3Position = Vector3::Add(xmf3Position, xmf3Look, fDistance);
	CGameObject::SetPosition(xmf3Position);
}

void CGameObject::Rotate(float fPitch, float fYaw, float fRoll)
{
	XMMATRIX mtxRotate = XMMatrixRotationRollPitchYaw(XMConvertToRadians(fPitch), XMConvertToRadians(fYaw), XMConvertToRadians(fRoll));
	m_xmf4x4Transform = Matrix4x4::Multiply(mtxRotate, m_xmf4x4Transform);

	UpdateTransform(NULL);
}

void CGameObject::Rotate(XMFLOAT3 *pxmf3Axis, float fAngle)
{
	XMMATRIX mtxRotate = XMMatrixRotationAxis(XMLoadFloat3(pxmf3Axis), XMConvertToRadians(fAngle));
	m_xmf4x4Transform = Matrix4x4::Multiply(mtxRotate, m_xmf4x4Transform);

	UpdateTransform(NULL);
}

void CGameObject::Rotate(XMFLOAT4 *pxmf4Quaternion)
{
	XMMATRIX mtxRotate = XMMatrixRotationQuaternion(XMLoadFloat4(pxmf4Quaternion));
	m_xmf4x4Transform = Matrix4x4::Multiply(mtxRotate, m_xmf4x4Transform);

	UpdateTransform(NULL);
}

#define _WITH_DEBUG_FRAME_HIERARCHY

void CGameObject::LoadFrameHierarchyFromFile(
	  CD3DDeviceIndRes*				pd3dDeviceIndRes
	, ID3D12GraphicsCommandList*	pd3dCommandList
	, ID3D12RootSignature*			pd3dGraphicsRootSignature
	, wifstream&					InFile
	, UINT							nFrame)
{
	static TCHAR pstrMeshName[256] = { 0 };
	static TCHAR pstrToken[256] = { 0 };
	static TCHAR pstrDebug[256] = { 0 };

	static XMFLOAT3 xmf3FramePosition;
	static XMFLOAT3 xmf3FrameRotation;
	static XMFLOAT3 xmf3FrameScale;
	static XMFLOAT4 xmf4FrameRotation;
	static XMFLOAT4 xmf4MaterialAlbedo;

	static XMFLOAT4X4 xmf4x4FrameTransform;
	
	XMFLOAT3*	pxmf3Positions = NULL;
	XMFLOAT3*	pxmf3Normals = NULL;
	XMFLOAT2*	pxmf3TextureCoords0 = NULL;
	XMFLOAT2*	pxmf3TextureCoords1 = NULL;
	UINT*		pIndices = NULL;

	UINT		nVertices = 0;
	UINT		nNormals = 0;
	UINT		nTextureCoords = 0;
	UINT		nIndices = 0;;

	while (true)
	{
		InFile >> pstrToken;
		if (!InFile) break;

		if (!_tcscmp(pstrToken, _T("FrameName:")))
		{
			InFile >> m_pstrFrameName;

			nVertices = nNormals = nTextureCoords = nIndices = 0;
			xmf4MaterialAlbedo = XMFLOAT4(-1.0f, -1.0f, -1.0f, -1.0f);
			pxmf3Positions = pxmf3Normals = NULL;
			pxmf3TextureCoords0 = pxmf3TextureCoords1 = NULL;
			pIndices = NULL;
		}
		else if (!_tcscmp(pstrToken, _T("Transform:")))
		{
			InFile >> xmf3FramePosition.x >> xmf3FramePosition.y >> xmf3FramePosition.z;
			InFile >> xmf3FrameRotation.x >> xmf3FrameRotation.y >> xmf3FrameRotation.z;
			InFile >> xmf4FrameRotation.x >> xmf4FrameRotation.y >> xmf4FrameRotation.z >> xmf4FrameRotation.w;
			InFile >> xmf3FrameScale.x >> xmf3FrameScale.y >> xmf3FrameScale.z;
		}
		else if (!_tcscmp(pstrToken, _T("TransformMatrix:")))
		{
			InFile >> xmf4x4FrameTransform._11 >> xmf4x4FrameTransform._12 >> xmf4x4FrameTransform._13 >> xmf4x4FrameTransform._14;
			InFile >> xmf4x4FrameTransform._21 >> xmf4x4FrameTransform._22 >> xmf4x4FrameTransform._23 >> xmf4x4FrameTransform._24;
			InFile >> xmf4x4FrameTransform._31 >> xmf4x4FrameTransform._32 >> xmf4x4FrameTransform._33 >> xmf4x4FrameTransform._34;
			InFile >> xmf4x4FrameTransform._41 >> xmf4x4FrameTransform._42 >> xmf4x4FrameTransform._43 >> xmf4x4FrameTransform._44;
		}
		else if (!_tcscmp(pstrToken, _T("MeshName:")))
		{
			InFile >> pstrMeshName;
		}
		else if (!_tcscmp(pstrToken, _T("Vertices:")))
		{
			InFile >> nVertices;
			pxmf3Positions = new XMFLOAT3[nVertices];
			for (int i = 0; i < nVertices; i++)
			{
				InFile >> pxmf3Positions[i].x >> pxmf3Positions[i].y >> pxmf3Positions[i].z;
			}
		}
		else if (!_tcscmp(pstrToken, _T("Normals:")))
		{
			InFile >> nNormals;
			pxmf3Normals = new XMFLOAT3[nNormals];
			for (int i = 0; i < nNormals; i++)
			{
				InFile >> pxmf3Normals[i].x >> pxmf3Normals[i].y >> pxmf3Normals[i].z;
			}
		}
		else if (!_tcscmp(pstrToken, _T("TextureCoordinates0:")))
		{
			InFile >> nTextureCoords;
			pxmf3TextureCoords0 = new XMFLOAT2[nTextureCoords];
			for (int i = 0; i < nTextureCoords; i++)
			{
				InFile >> pxmf3TextureCoords0[i].x >> pxmf3TextureCoords0[i].y;
			}
		}
		else if (!_tcscmp(pstrToken, _T("TextureCoordinates1:")))
		{
			InFile >> nTextureCoords;
			pxmf3TextureCoords1 = new XMFLOAT2[nTextureCoords];
			for (int i = 0; i < nTextureCoords; i++)
			{
				InFile >> pxmf3TextureCoords1[i].x >> pxmf3TextureCoords1[i].y;
			}
		}
		else if (!_tcscmp(pstrToken, _T("Indices:")))
		{
			InFile >> nIndices;
			pIndices = new UINT[nIndices];
			for (int i = 0; i < nIndices; i++)
			{
				InFile >> pIndices[i];
			}
		}
		else if (!_tcscmp(pstrToken, _T("AlbedoColor:")))
		{
			InFile >> xmf4MaterialAlbedo.x >> xmf4MaterialAlbedo.y >> xmf4MaterialAlbedo.z >> xmf4MaterialAlbedo.w;
		}
		else if (!_tcscmp(pstrToken, _T("Children:")))
		{
			int nChilds = 0;
			InFile >> nChilds;
			if (nChilds > 0)
			{
				for (int i = 0; i < nChilds; i++)
				{
					CGameObject *pChild = new CGameObject(1);
					pChild->LoadFrameHierarchyFromFile(pd3dDeviceIndRes, pd3dCommandList, pd3dGraphicsRootSignature, InFile, nFrame + 1);
					SetChild(pChild);
#ifdef _WITH_DEBUG_FRAME_HIERARCHY
					_stprintf_s(pstrDebug, 256, _T("(Frame: %p) (Parent: %p)\n"), pChild, this);
					OutputDebugString(pstrDebug);
#endif
				}
			}
		}
		else if (!_tcscmp(pstrToken, _T("EndOfFrame")))
		{
			CMesh *pMesh = NULL;
			CMaterial *pMaterial = NULL;
			if (nVertices > 0)
			{
				CVertex* pVertices = new CVertex[nVertices];
				for (int i = 0; i < nVertices; ++i)
				{
					pVertices[i].m_xmf3Position = pxmf3Positions[i];
					if (nNormals > 0)
						pVertices[i].m_xmf3Normal = pxmf3Normals[i];
					if (nTextureCoords > 0)
						pVertices[i].m_xmf2TexCoord0 = pxmf3TextureCoords0[i];
				}
				pMaterial = new CMaterial();
				pMaterial->m_xmf4Albedo = xmf4MaterialAlbedo;

				UINT ncbElementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255);

				CreateShaderVariables(pd3dDeviceIndRes, pd3dCommandList);

				CIlluminatedShader *pShader = new CIlluminatedShader();
				pShader->CreateShader(pd3dDeviceIndRes, pd3dGraphicsRootSignature);
				pShader->CreateShaderVariables(pd3dDeviceIndRes, pd3dCommandList);
				pShader->CreateCbvAndSrvDescriptorHeaps(pd3dDeviceIndRes, pd3dCommandList, 1, 0);
				pShader->CreateConstantBufferViews(pd3dDeviceIndRes, pd3dCommandList, 1, m_pd3dcbGameObject.Get(), ncbElementBytes);

				SetCbvGPUDescriptorHandle(pShader->GetGPUCbvDescriptorStartHandle());

				pMaterial->SetShader(pShader);

				pMesh = new CMesh(pd3dDeviceIndRes, pd3dCommandList, nVertices, pVertices, nIndices, pIndices);
				delete[] pVertices;
			}

			if (pMesh)
				SetMesh(0, pMesh);
			else
				ResizeMeshes(0);
			if (pMaterial) SetMaterial(pMaterial);

			m_xmf4x4World = m_xmf4x4Transform = xmf4x4FrameTransform;

			if (pxmf3Positions) delete[] pxmf3Positions;
			if (pxmf3Normals) delete[] pxmf3Normals;
			if (pxmf3TextureCoords0) delete[] pxmf3TextureCoords0;
			if (pxmf3TextureCoords1) delete[] pxmf3TextureCoords1;
			if (pIndices) delete[] pIndices;

			break;
		}
	}
}

void CGameObject::LoadGeometryFromFile(
	  CD3DDeviceIndRes*				pd3dDeviceIndRes
	, ID3D12GraphicsCommandList*	pd3dCommandList
	, ID3D12RootSignature*			pd3dGraphicsRootSignature
	, TCHAR*						pstrFileName)
{
	wifstream InFile(pstrFileName);
	LoadFrameHierarchyFromFile(
		  pd3dDeviceIndRes
		, pd3dCommandList
		, pd3dGraphicsRootSignature
		, InFile
		, 0);

#ifdef _WITH_DEBUG_FRAME_HIERARCHY
	TCHAR pstrDebug[256] = { 0 };
	_stprintf_s(pstrDebug, 256, _T("Frame Hierarchy\n"));
	OutputDebugString(pstrDebug);

	PrintFrameInfo(this, NULL);
#endif
}

void CGameObject::PrintFrameInfo(CGameObject* pGameObject, CGameObject * pParent)
{
	TCHAR pstrDebug[256] = { 0 };

	_stprintf_s(pstrDebug, 256, _T("(Frame: %p) (Parent: %p)\n"), pGameObject, pParent);
	OutputDebugString(pstrDebug);

	if (pGameObject->m_pSibling) PrintFrameInfo(pGameObject->m_pSibling, pParent);
	if (pGameObject->m_pChild) PrintFrameInfo(pGameObject->m_pChild, pGameObject);
}

CApacheHellicopter::CApacheHellicopter(
	  CD3DDeviceIndRes*				pd3dDeviceIndRes
	, ID3D12GraphicsCommandList*	pd3dCommandList
	, ID3D12RootSignature*			pd3dGraphicsRootSignature)
{
	LoadGeometryFromFile(
		  pd3dDeviceIndRes
		, pd3dCommandList
		, pd3dGraphicsRootSignature
		, L"../Assets/Model/Apache.txt");
	m_pRotorFrame = FindFrame(_T("rotor"));
}

CApacheHellicopter::~CApacheHellicopter()
{
}

void CApacheHellicopter::Animate(float fTimeElapsed)
{
	if (m_pRotorFrame)
	{
		XMFLOAT3 xmf3RotationAxis = Vector3::Normalize(XMFLOAT3(m_pRotorFrame->m_xmf4x4Transform._21, m_pRotorFrame->m_xmf4x4Transform._22, m_pRotorFrame->m_xmf4x4Transform._23));
		XMMATRIX xmmtxRotate = XMMatrixRotationAxis(XMLoadFloat3(&xmf3RotationAxis), XMConvertToRadians(360.0f * 3.0f) * fTimeElapsed);
		m_pRotorFrame->m_xmf4x4World = Matrix4x4::Multiply(xmmtxRotate, m_pRotorFrame->m_xmf4x4World);
	}
}

CFlyerShip::CFlyerShip(
	  CD3DDeviceIndRes*				pd3dDeviceIndRes
	, ID3D12GraphicsCommandList*	pd3dCommandList
	, ID3D12RootSignature*			pd3dGraphicsRootSignature)
{
	LoadGeometryFromFile(
		  pd3dDeviceIndRes
		, pd3dCommandList
		, pd3dGraphicsRootSignature
		, L"../Assets/Model/Flyer.txt");

	UINT ncbElementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255);

	CreateShaderVariables(pd3dDeviceIndRes, pd3dCommandList);

	CIlluminatedShader *pShader = new CIlluminatedShader();
	pShader->CreateShader(pd3dDeviceIndRes, pd3dGraphicsRootSignature);
	pShader->CreateShaderVariables(pd3dDeviceIndRes, pd3dCommandList);
	pShader->CreateCbvAndSrvDescriptorHeaps(pd3dDeviceIndRes, pd3dCommandList, 1, 2);
	pShader->CreateConstantBufferViews(pd3dDeviceIndRes, pd3dCommandList, 1, m_pd3dcbGameObject.Get(), ncbElementBytes);

	SetCbvGPUDescriptorHandle(pShader->GetGPUCbvDescriptorStartHandle());

	SetShader(pShader);
}

CFlyerShip::~CFlyerShip()
{
}


CHeightMapTerrain::CHeightMapTerrain(
	  CD3DDeviceIndRes* pd3dDeviceIndRes
	, ID3D12GraphicsCommandList *pd3dCommandList
	, ID3D12RootSignature *pd3dGraphicsRootSignature
	, LPCTSTR pFileName, int nWidth, int nLength
	, int nBlockWidth, int nBlockLength
	, XMFLOAT3 xmf3Scale, XMFLOAT4 xmf4Color) 
	: CGameObject(0)
{
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

	CHeightMapGridMesh* pHeightMapGridMesh = NULL;
	for (int z = 0, zStart = 0; z < czBlocks; z++)
	{
		for (int x = 0, xStart = 0; x < cxBlocks; x++)
		{
			xStart = x * (nBlockWidth - 1);
			zStart = z * (nBlockLength - 1);
			pHeightMapGridMesh = new CHeightMapGridMesh(
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

	CTerrainShader *pShader = new CTerrainShader();
	pShader->CreateShader(pd3dDeviceIndRes, pd3dGraphicsRootSignature);
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
	  CD3DDeviceIndRes*				pd3dDeviceIndRes
	, ID3D12GraphicsCommandList*	pd3dCommandList
	, ID3D12RootSignature*			pd3dGraphicsRootSignature) 
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
