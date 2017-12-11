#include "stdafx.h"
#include "Player.h"
#include "GameFramework.h"
#include "Scene.h"

CScene::CScene()
{
	m_nCurrentBulletIdx = 0;
	m_nCurrentParticleIdx = 0;
	m_fBulletTimer = 0.0f;
	m_fRestartCounter = 0.0f;
	m_ptOldCursorPos = { 0, 0 };
}
CScene::~CScene()
{
}

void CScene::SetHWNDAndHInstance(HWND hWnd, HINSTANCE hInstance)
{
	m_hWnd = hWnd;
	m_hInstance = hInstance;
}

ID3D12RootSignature* CScene::GetGraphicsRootSignature()
{
	return(m_pd3dGraphicsRootSignature.Get());
}
ID3D12RootSignature* CScene::CreateGraphicsRootSignature(CD3DDeviceIndRes* pd3dDeviceIndRes)
{
	ID3D12RootSignature* pd3dGraphicsRootSignature = NULL;
	D3D12_DESCRIPTOR_RANGE pd3dDescriptorRanges[4];

	pd3dDescriptorRanges[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	pd3dDescriptorRanges[0].NumDescriptors = 6;
	pd3dDescriptorRanges[0].BaseShaderRegister = 1; //t1: gtxtTextures[6]
	pd3dDescriptorRanges[0].RegisterSpace = 0;
	pd3dDescriptorRanges[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	pd3dDescriptorRanges[1].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	pd3dDescriptorRanges[1].NumDescriptors = 1;
	pd3dDescriptorRanges[1].BaseShaderRegister = 7; //t7: gtxtTerrainBaseTexture
	pd3dDescriptorRanges[1].RegisterSpace = 0;
	pd3dDescriptorRanges[1].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	pd3dDescriptorRanges[2].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	pd3dDescriptorRanges[2].NumDescriptors = 1;
	pd3dDescriptorRanges[2].BaseShaderRegister = 8; //t8: gtxtTerrainDetailTexture
	pd3dDescriptorRanges[2].RegisterSpace = 0;
	pd3dDescriptorRanges[2].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	pd3dDescriptorRanges[3].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	pd3dDescriptorRanges[3].NumDescriptors = 1;
	pd3dDescriptorRanges[3].BaseShaderRegister = 9; //t9: gtxtSkyBoxTexture
	pd3dDescriptorRanges[3].RegisterSpace = 0;
	pd3dDescriptorRanges[3].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	D3D12_ROOT_PARAMETER pd3dRootParameters[6];
	pd3dRootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	pd3dRootParameters[0].Descriptor.ShaderRegister = 0; //Player
	pd3dRootParameters[0].Descriptor.RegisterSpace = 0;
	pd3dRootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	pd3dRootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	pd3dRootParameters[1].Descriptor.ShaderRegister = 1; //Camera
	pd3dRootParameters[1].Descriptor.RegisterSpace = 0;
	pd3dRootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	pd3dRootParameters[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_SRV;
	pd3dRootParameters[2].Descriptor.ShaderRegister = 0; //t0
	pd3dRootParameters[2].Descriptor.RegisterSpace = 0;
	pd3dRootParameters[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	pd3dRootParameters[3].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	pd3dRootParameters[3].Descriptor.ShaderRegister = 3; //Materials
	pd3dRootParameters[3].Descriptor.RegisterSpace = 0;
	pd3dRootParameters[3].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	pd3dRootParameters[4].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	pd3dRootParameters[4].Descriptor.ShaderRegister = 4; //Lights
	pd3dRootParameters[4].Descriptor.RegisterSpace = 0;
	pd3dRootParameters[4].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	pd3dRootParameters[5].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	pd3dRootParameters[5].DescriptorTable.NumDescriptorRanges = 4;
	pd3dRootParameters[5].DescriptorTable.pDescriptorRanges = pd3dDescriptorRanges;
	pd3dRootParameters[5].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	D3D12_ROOT_SIGNATURE_FLAGS d3dRootSignatureFlags =
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS;
		//| D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS;
		//| D3D12_ROOT_SIGNATURE_FLAG_DENY_PIXEL_SHADER_ROOT_ACCESS;

	D3D12_STATIC_SAMPLER_DESC pd3dSamplerDescs[2];
	pd3dSamplerDescs[0].Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
	pd3dSamplerDescs[0].AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	pd3dSamplerDescs[0].AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	pd3dSamplerDescs[0].AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	pd3dSamplerDescs[0].MipLODBias = 0;
	pd3dSamplerDescs[0].MaxAnisotropy = 1;
	pd3dSamplerDescs[0].ComparisonFunc = D3D12_COMPARISON_FUNC_ALWAYS;
	pd3dSamplerDescs[0].MinLOD = 0;
	pd3dSamplerDescs[0].MaxLOD = D3D12_FLOAT32_MAX;
	pd3dSamplerDescs[0].ShaderRegister = 0;
	pd3dSamplerDescs[0].RegisterSpace = 0;
	pd3dSamplerDescs[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	pd3dSamplerDescs[1].Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
	pd3dSamplerDescs[1].AddressU = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
	pd3dSamplerDescs[1].AddressV = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
	pd3dSamplerDescs[1].AddressW = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
	pd3dSamplerDescs[1].MipLODBias = 0;
	pd3dSamplerDescs[1].MaxAnisotropy = 1;
	pd3dSamplerDescs[1].ComparisonFunc = D3D12_COMPARISON_FUNC_ALWAYS;
	pd3dSamplerDescs[1].MinLOD = 0;
	pd3dSamplerDescs[1].MaxLOD = D3D12_FLOAT32_MAX;
	pd3dSamplerDescs[1].ShaderRegister = 1;
	pd3dSamplerDescs[1].RegisterSpace = 0;
	pd3dSamplerDescs[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	D3D12_ROOT_SIGNATURE_DESC d3dRootSignatureDesc;
	::ZeroMemory(&d3dRootSignatureDesc, sizeof(D3D12_ROOT_SIGNATURE_DESC));
	d3dRootSignatureDesc.NumParameters = _countof(pd3dRootParameters);
	d3dRootSignatureDesc.pParameters = pd3dRootParameters;
	d3dRootSignatureDesc.NumStaticSamplers = _countof(pd3dSamplerDescs);
	d3dRootSignatureDesc.pStaticSamplers = pd3dSamplerDescs;
	d3dRootSignatureDesc.Flags = d3dRootSignatureFlags;

	ComPtr<ID3DBlob> pd3dSignatureBlob = NULL;
	ComPtr<ID3DBlob> pd3dErrorBlob = NULL;
	D3D12SerializeRootSignature(
		  &d3dRootSignatureDesc
		, D3D_ROOT_SIGNATURE_VERSION_1
		, &pd3dSignatureBlob
		, &pd3dErrorBlob);

	pd3dDeviceIndRes->CreateRootSignature(
		  0
		, pd3dSignatureBlob->GetBufferPointer()
		, pd3dSignatureBlob->GetBufferSize()
		, &pd3dGraphicsRootSignature);

	return(pd3dGraphicsRootSignature);
}

void CScene::CreateShaderVariables(CD3DDeviceIndRes* pd3dDeviceIndRes, ID3D12GraphicsCommandList *pd3dCommandList)
{
	UINT ncbElementBytes = ((sizeof(LIGHTS) + 255) & ~255); //256의 배수
	m_pd3dcbLights = pd3dDeviceIndRes->CreateBufferResource(
		pd3dCommandList
		, NULL
		, ncbElementBytes
		, D3D12_HEAP_TYPE_UPLOAD
		, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER
		, NULL);

	m_pd3dcbLights->Map(0, NULL, (void **)&m_pcbMappedLights);

	UINT ncbMaterialBytes = ((sizeof(MATERIALS) + 255) & ~255); //256의 배수
	m_pd3dcbMaterials = pd3dDeviceIndRes->CreateBufferResource(
		pd3dCommandList
		, NULL
		, ncbMaterialBytes
		, D3D12_HEAP_TYPE_UPLOAD
		, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER
		, NULL);

	m_pd3dcbMaterials->Map(0, NULL, (void **)&m_pcbMappedMaterials);
}

void CScene::UpdateShaderVariables(ID3D12GraphicsCommandList *pd3dCommandList)
{
	::memcpy(m_pcbMappedLights, m_pLights, sizeof(LIGHTS));
	::memcpy(m_pcbMappedMaterials, m_pMaterials, sizeof(MATERIALS));
}

void CScene::ReleaseShaderVariables()
{
	if (m_pd3dcbLights)
	{
		m_pd3dcbLights->Unmap(0, NULL);
	}
	if (m_pd3dcbMaterials)
	{
		m_pd3dcbMaterials->Unmap(0, NULL);
	}
}

void CScene::BuildLightsAndMaterials()
{
	m_pLights = new LIGHTS;
	::ZeroMemory(m_pLights, sizeof(LIGHTS));

	m_pLights->m_xmf4GlobalAmbient = XMFLOAT4(0.005f, 0.005f, 0.005f, 1.0f);

	// 플레이어 손전등
	m_pLights->m_pLights[0].m_bEnable = true;
	m_pLights->m_pLights[0].m_nType = SPOT_LIGHT;
	m_pLights->m_pLights[0].m_fRange = 300.0f;
	m_pLights->m_pLights[0].m_xmf4Ambient = XMFLOAT4(0.1f, 0.1f, 0.1f, 1.0f);
	m_pLights->m_pLights[0].m_xmf4Diffuse = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
	m_pLights->m_pLights[0].m_xmf4Specular = XMFLOAT4(0.3f, 0.3f, 0.3f, 0.0f);
	m_pLights->m_pLights[0].m_xmf3Position = XMFLOAT3(-50.0f, 20.0f, -5.0f);
	m_pLights->m_pLights[0].m_xmf3Direction = XMFLOAT3(0.0f, 0.0f, 1.0f);
	m_pLights->m_pLights[0].m_xmf3Attenuation = XMFLOAT3(1.0f, 0.01f, 0.0001f);
	m_pLights->m_pLights[0].m_fFalloff = 9.0f;
	m_pLights->m_pLights[0].m_fPhi = (float)cos(XMConvertToRadians(70.0f));
	m_pLights->m_pLights[0].m_fTheta = (float)cos(XMConvertToRadians(35.0f));

	// 태양
	m_pLights->m_pLights[1].m_bEnable = true;
	m_pLights->m_pLights[1].m_nType = DIRECTIONAL_LIGHT;
	m_pLights->m_pLights[1].m_xmf4Ambient = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);
	m_pLights->m_pLights[1].m_xmf4Diffuse = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
	m_pLights->m_pLights[1].m_xmf4Specular = XMFLOAT4(0.3f, 0.3f, 0.3f, 0.0f);
	m_pLights->m_pLights[1].m_xmf3Direction = XMFLOAT3(0.0f, -1.0f, 0.0f);
	m_pLights->m_pLights[1].m_xmf3Position = XMFLOAT3(-50.0f, 1020.0f, -5.0f);

	// 달
	m_pLights->m_pLights[2].m_bEnable = true;
	m_pLights->m_pLights[2].m_nType = POINT_LIGHT;
	m_pLights->m_pLights[2].m_fRange = 2000.0f;
	m_pLights->m_pLights[2].m_xmf4Ambient = XMFLOAT4(0.1f, 0.1f, 0.1f, 1.0f);
	m_pLights->m_pLights[2].m_xmf4Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	m_pLights->m_pLights[2].m_xmf4Specular = XMFLOAT4(0.1f, 0.1f, 0.1f, 0.0f);
	m_pLights->m_pLights[2].m_xmf3Position = XMFLOAT3(130.0f, 30.0f, 30.0f);
	m_pLights->m_pLights[2].m_xmf3Direction = XMFLOAT3(0.0f, 0.0f, 0.0f);
	m_pLights->m_pLights[2].m_xmf3Attenuation = XMFLOAT3(1.0f, 0.00001f, 0.000001f);

	m_pMaterials = new MATERIALS;
	::ZeroMemory(m_pMaterials, sizeof(MATERIALS));

	m_pMaterials->m_pReflections[0] = { XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f), XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 5.0f), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f) };
	m_pMaterials->m_pReflections[1] = { XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f), XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 10.0f), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f) };
	m_pMaterials->m_pReflections[2] = { XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f), XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 15.0f), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f) };
	m_pMaterials->m_pReflections[3] = { XMFLOAT4(0.5f, 0.0f, 1.0f, 1.0f), XMFLOAT4(0.0f, 0.5f, 1.0f, 1.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 20.0f), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f) };
	m_pMaterials->m_pReflections[4] = { XMFLOAT4(0.0f, 0.5f, 1.0f, 1.0f), XMFLOAT4(0.5f, 0.0f, 1.0f, 1.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 25.0f), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f) };
	m_pMaterials->m_pReflections[5] = { XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 30.0f), XMFLOAT4(1.0f, 1.0f, 0.0f, 1.0f) };
	m_pMaterials->m_pReflections[6] = { XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 35.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f) };
	m_pMaterials->m_pReflections[7] = { XMFLOAT4(0.0f, 0.8f, 0.0f, 1.0f), XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 40.0f), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f) };
}
void CScene::BuildObjects(
	CD3DDeviceIndRes* pd3dDeviceIndRes
	, ID3D12GraphicsCommandList *pd3dCommandList)
{
	m_pd3dGraphicsRootSignature = CreateGraphicsRootSignature(pd3dDeviceIndRes);

	// Terrain
	{
		XMFLOAT3 xmf3Scale(6.5f, 2.0f, 6.5f);
		XMFLOAT4 xmf4Color(0.0f, 0.2f, 0.0f, 0.0f);
#ifdef _WITH_TERRAIN_PARTITION
		m_pTerrain = new CHeightMapTerrain(
			pd3dDeviceIndRes
			, pd3dCommandList
			, m_pd3dGraphicsRootSignature.Get()
			, _T("Assets/Image/Terrain/HeightMap.raw")
			, 257, 257, 17, 17, xmf3Scale, xmf4Color);
#else
		m_pTerrain = new CHeightMapTerrain(
			pd3dDeviceIndRes
			, pd3dCommandList
			, m_pd3dGraphicsRootSignature.Get()
			, _T("Assets/Image/Terrain/HeightMap.raw")
			, 257, 257, 257, 257, xmf3Scale, xmf4Color);
#endif
	}

	// SkyBox
	{
		m_pSkyBox = new CSkyBox(
			pd3dDeviceIndRes
			, pd3dCommandList
			, m_pd3dGraphicsRootSignature.Get());
	}

	// Tree
	{
		m_pTrees = new CBillBoardShader();
		m_pTrees->CreateShader(pd3dDeviceIndRes, m_pd3dGraphicsRootSignature.Get());
		int nTrees = NUM_TREES;
		CBillBoardVertex* pvtxTrees = new CBillBoardVertex[nTrees];
		XMFLOAT3 xmf3Pos;
		XMFLOAT2 xmfSize = XMFLOAT2(20, 50);
		for (int i = 0; i < nTrees; ++i)
		{
			xmf3Pos.x = (float)RANDOM_NUM(800, 1500);
			xmf3Pos.z = (float)RANDOM_NUM(800, 1500);
			xmf3Pos.y = m_pTerrain->GetHeight(
				xmf3Pos.x, xmf3Pos.z) + xmfSize.y / 2.f;
			pvtxTrees[i].m_xmf4Pos = xmf3Pos;
			pvtxTrees[i].m_xmf2Size = xmfSize;
		}
		m_pTrees->BuildObjects(
			pd3dDeviceIndRes
			, pd3dCommandList
			, pvtxTrees
			, nTrees);

		CTexture *pTexture = new CTexture(6, RESOURCE_TEXTURE2D, 0);
		pTexture->LoadTextureFromFile(pd3dDeviceIndRes, pd3dCommandList, L"Assets/Image/Trees/Tree01.dds", 0);
		pTexture->LoadTextureFromFile(pd3dDeviceIndRes, pd3dCommandList, L"Assets/Image/Trees/Tree02.dds", 1);
		pTexture->LoadTextureFromFile(pd3dDeviceIndRes, pd3dCommandList, L"Assets/Image/Trees/Tree03.dds", 2);
		pTexture->LoadTextureFromFile(pd3dDeviceIndRes, pd3dCommandList, L"Assets/Image/Trees/Tree04.dds", 3);
		pTexture->LoadTextureFromFile(pd3dDeviceIndRes, pd3dCommandList, L"Assets/Image/Trees/Tree06.dds", 4);
		pTexture->LoadTextureFromFile(pd3dDeviceIndRes, pd3dCommandList, L"Assets/Image/Trees/Tree08.dds", 5);

		m_pTrees->SetTexture(pTexture);
		m_pTrees->CreateShaderVariables(pd3dDeviceIndRes, pd3dCommandList);
		m_pTrees->CreateCbvAndSrvDescriptorHeaps(pd3dDeviceIndRes, pd3dCommandList, 0, 6);
		m_pTrees->CreateShaderResourceViews(pd3dDeviceIndRes, pd3dCommandList, pTexture, 5, false);
	}

	// Grass
	{
		m_pGrasses = new CBillBoardShader();
		m_pGrasses->CreateShader(pd3dDeviceIndRes, m_pd3dGraphicsRootSignature.Get());
		int nGrasses = NUM_GRASSES;
		CBillBoardVertex* pvtxGrasses = new CBillBoardVertex[nGrasses];
		XMFLOAT3 xmf3Pos;
		XMFLOAT2 xmfSize = XMFLOAT2(20, 10);
		for (int i = 0; i < nGrasses; ++i)
		{
			xmf3Pos.x = (float)RANDOM_NUM(800, 1500);
			xmf3Pos.z = (float)RANDOM_NUM(0, 800);
			xmf3Pos.y = m_pTerrain->GetHeight(
				xmf3Pos.x, xmf3Pos.z) + xmfSize.y / 2.f;
			pvtxGrasses[i].m_xmf4Pos = xmf3Pos;
			pvtxGrasses[i].m_xmf2Size = xmfSize;
		}
		m_pGrasses->BuildObjects(
			pd3dDeviceIndRes
			, pd3dCommandList
			, pvtxGrasses
			, nGrasses);

		CTexture *pTexture = new CTexture(6, RESOURCE_TEXTURE2D, 0);
		pTexture->LoadTextureFromFile(pd3dDeviceIndRes, pd3dCommandList, L"Assets/Image/Grasses/Grass01.DDS", 0);
		pTexture->LoadTextureFromFile(pd3dDeviceIndRes, pd3dCommandList, L"Assets/Image/Grasses/Grass02.DDS", 1);
		pTexture->LoadTextureFromFile(pd3dDeviceIndRes, pd3dCommandList, L"Assets/Image/Grasses/Grass08.DDS", 2);
		pTexture->LoadTextureFromFile(pd3dDeviceIndRes, pd3dCommandList, L"Assets/Image/Grasses/Grass12.DDS", 3);
		pTexture->LoadTextureFromFile(pd3dDeviceIndRes, pd3dCommandList, L"Assets/Image/Flowers/Flower07.DDS", 4);
		pTexture->LoadTextureFromFile(pd3dDeviceIndRes, pd3dCommandList, L"Assets/Image/Flowers/Flower06.DDS", 5);

		m_pGrasses->SetTexture(pTexture);
		m_pGrasses->CreateShaderVariables(pd3dDeviceIndRes, pd3dCommandList);
		m_pGrasses->CreateCbvAndSrvDescriptorHeaps(pd3dDeviceIndRes, pd3dCommandList, 0, 6);
		m_pGrasses->CreateShaderResourceViews(pd3dDeviceIndRes, pd3dCommandList, pTexture, 5, false);
	}


	BuildLightsAndMaterials();

	CreateShaderVariables(pd3dDeviceIndRes, pd3dCommandList);
}

void CScene::ReleaseObjects()
{
	for (int i = 0; i < m_nShaders; i++)
	{
		m_pShaders[i].ReleaseShaderVariables();
		m_pShaders[i].ReleaseObjects();
	}
	if (m_pShaders) delete[] m_pShaders;
	if (m_pTerrain) delete m_pTerrain;

	ReleaseShaderVariables();

	if (m_pLights) delete m_pLights;
	if (m_pMaterials) delete m_pMaterials;
	if (m_pSkyBox) delete m_pSkyBox;

	if (m_pTrees) delete m_pTrees;
	if (m_pGrasses) delete m_pGrasses;
}

void CScene::Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera)
{
	pd3dCommandList->SetGraphicsRootSignature(m_pd3dGraphicsRootSignature.Get());

	pCamera->SetViewportsAndScissorRects(pd3dCommandList);
	pCamera->UpdateShaderVariables(pd3dCommandList);
	UpdateShaderVariables(pd3dCommandList);

	D3D12_GPU_VIRTUAL_ADDRESS d3dcbLightsGpuVirtualAddress = m_pd3dcbLights->GetGPUVirtualAddress();
	pd3dCommandList->SetGraphicsRootConstantBufferView(4, d3dcbLightsGpuVirtualAddress); //Lights

	D3D12_GPU_VIRTUAL_ADDRESS d3dcbMaterialsGpuVirtualAddress = m_pd3dcbMaterials->GetGPUVirtualAddress();
	pd3dCommandList->SetGraphicsRootConstantBufferView(3, d3dcbMaterialsGpuVirtualAddress);

	if (m_pSkyBox) m_pSkyBox->Render(pd3dCommandList, pCamera);
	if (m_pTerrain) m_pTerrain->Render(pd3dCommandList, pCamera);
	if (m_pGrasses) m_pGrasses->Render(pd3dCommandList, pCamera);
	if (m_pTrees) m_pTrees->Render(pd3dCommandList, pCamera);
	for (int i = 0; i < m_nShaders; i++)
		m_pShaders[i].Render(pd3dCommandList, pCamera);
}
bool CScene::ProcessInput(float fTimeElapsed)
{
	if (!m_pPlayer) return false;
	static UCHAR pKeyBuffer[256];
	DWORD dwDirection = 0;

	if (::GetKeyboardState(pKeyBuffer))
	{
		if (pKeyBuffer['W'] & 0xF0) dwDirection |= DIR_FORWARD;
		if (pKeyBuffer['S'] & 0xF0) dwDirection |= DIR_BACKWARD;
		if (pKeyBuffer['A'] & 0xF0) dwDirection |= DIR_LEFT;
		if (pKeyBuffer['D'] & 0xF0) dwDirection |= DIR_RIGHT;
		if (pKeyBuffer['Q'] & 0xF0) dwDirection |= DIR_UP;
		if (pKeyBuffer['E'] & 0xF0) dwDirection |= DIR_DOWN;
		if (pKeyBuffer['F'] & 0xF0)
			m_pLights->m_pLights[0].m_bEnable = true;
		if (pKeyBuffer['G'] & 0xF0)
			m_pLights->m_pLights[0].m_bEnable = false;
		if (pKeyBuffer[VK_SPACE] & 0xF0)
			ShootBullet();
	}

	float cxDelta = 0.0f, cyDelta = 0.0f;
	POINT ptCursorPos;
	if (::GetCapture() == m_hWnd)
	{
		//마우스 커서를 화면에서 없앤다(보이지 않게 한다).
		::SetCursor(NULL);
		::GetCursorPos(&ptCursorPos);
		cxDelta = (float)(ptCursorPos.x - m_ptOldCursorPos.x) / 3.0f;
		cyDelta = (float)(ptCursorPos.y - m_ptOldCursorPos.y) / 3.0f;
		::SetCursorPos(m_ptOldCursorPos.x, m_ptOldCursorPos.y);
	}

	if ((dwDirection != 0) || (cxDelta != 0.0f) || (cyDelta != 0.0f))
	{
		
		if (cxDelta || cyDelta)
		{
			if (pKeyBuffer[VK_RBUTTON] & 0xF0)
				m_pPlayer->Rotate(cyDelta, 0.0f, -cxDelta);
			else
				m_pPlayer->Rotate(cyDelta, cxDelta, 0.0f);
		}
		if (dwDirection)
			m_pPlayer->Move(dwDirection, 50.0f * fTimeElapsed, true);
	}

	m_pPlayer->Update(fTimeElapsed);
	return(true);
}
void CScene::AnimateObjects(float fTimeElapsed)
{
	XMFLOAT3 PlayerPos = m_pPlayer->GetPosition();
	if (m_bGameOver)
	{
		m_fRestartCounter += fTimeElapsed;
		if (m_fRestartCounter > 2)
		{
			m_fRestartCounter = 0.0f;
			m_bGameOver = false;
			m_pPlayer->SetPosition(XMFLOAT3(800, 300, 800));
		}
	}
	m_fBulletTimer += fTimeElapsed;
	for (int i = 0; i < m_nShaders; i++)
		m_pShaders[i].AnimateObjects(fTimeElapsed);
	m_pPlayer->Animate(fTimeElapsed);

	PhysicsProcessing(fTimeElapsed);

	for (int i = 0; i < m_nObjects; ++i)
		{
		CGameObject* Object = m_ppObjects[i];
		XMFLOAT3 ObjectPos = Object->GetPosition();
		float height = m_pTerrain->GetHeight(ObjectPos.x, ObjectPos.z) + 6.0f;
		if(ObjectPos.y < height)
		Object->SetPosition(ObjectPos.x, height, ObjectPos.z);
	}

	if (m_pLights)
	{
		m_pLights->m_pLights[0].m_xmf3Position = m_pPlayer->GetPosition();
		m_pLights->m_pLights[0].m_xmf3Direction = m_pPlayer->GetLookVector();
	}
}

void CScene::PhysicsProcessing(float fTimeElapsed)
{
	
}

void CScene::ReleaseUploadBuffers()
{
	for (int i = 0; i < m_nShaders; i++) 
		m_pShaders[i].ReleaseUploadBuffers();
	if (m_pTerrain) m_pTerrain->ReleaseUploadBuffers();
}

bool CScene::OnProcessingMouseMessage(
	HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	switch (nMessageID)
	{
	case WM_LBUTTONDOWN:
	case WM_RBUTTONDOWN:
		//마우스가 눌려지면 마우스 픽킹을 하여 선택한 게임 객체를 찾는다.
		//m_pSelectedObject = PickObjectPointedByCursor(LOWORD(lParam),
		//	HIWORD(lParam), m_pPlayer->GetCamera());
		//마우스 캡쳐를 하고 현재 마우스 위치를 가져온다.
		::SetCapture(hWnd);
		::GetCursorPos(&m_ptOldCursorPos);
		break;
	case WM_LBUTTONUP:
		//if(m_pSelectedObject)
		//	ShootBullet();
	case WM_RBUTTONUP:
		//마우스 캡쳐를 해제한다.
		::ReleaseCapture();
		break;
	case WM_MOUSEMOVE:
		break;
	default:
		break;
	}
	return(false);
}
bool CScene::OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam,
	LPARAM lParam)
{
	return(false);
}

void CScene::ShootBullet()
{
	if (m_fBulletTimer > BulletDelay)
	{
		m_fBulletTimer = 0.0f;
		
	}
}