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
ID3D12RootSignature* CScene::CreateGraphicsRootSignature(CD3DDeviceIndRes *pd3dDeviceIndRes)
{
	ID3D12RootSignature* pd3dGraphicsRootSignature = NULL;
	D3D12_ROOT_PARAMETER pd3dRootParameters[5];
	pd3dRootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	pd3dRootParameters[0].Descriptor.ShaderRegister = 0; //Player
	pd3dRootParameters[0].Descriptor.RegisterSpace = 0;
	pd3dRootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;

	pd3dRootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	pd3dRootParameters[1].Descriptor.ShaderRegister = 1; //Camera
	pd3dRootParameters[1].Descriptor.RegisterSpace = 0;
	pd3dRootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	pd3dRootParameters[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_SRV;
	pd3dRootParameters[2].Descriptor.ShaderRegister = 0; //t0
	pd3dRootParameters[2].Descriptor.RegisterSpace = 0;
	pd3dRootParameters[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;

	pd3dRootParameters[3].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	pd3dRootParameters[3].Descriptor.ShaderRegister = 3; //Materials
	pd3dRootParameters[3].Descriptor.RegisterSpace = 0;
	pd3dRootParameters[3].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	pd3dRootParameters[4].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	pd3dRootParameters[4].Descriptor.ShaderRegister = 4; //Lights
	pd3dRootParameters[4].Descriptor.RegisterSpace = 0;
	pd3dRootParameters[4].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	D3D12_ROOT_SIGNATURE_FLAGS d3dRootSignatureFlags =
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_PIXEL_SHADER_ROOT_ACCESS;

	D3D12_ROOT_SIGNATURE_DESC d3dRootSignatureDesc;
	::ZeroMemory(&d3dRootSignatureDesc, sizeof(D3D12_ROOT_SIGNATURE_DESC));
	d3dRootSignatureDesc.NumParameters = _countof(pd3dRootParameters);
	d3dRootSignatureDesc.pParameters = pd3dRootParameters;
	d3dRootSignatureDesc.NumStaticSamplers = 0;
	d3dRootSignatureDesc.pStaticSamplers = NULL;
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

void CScene::CreateShaderVariables(CD3DDeviceIndRes *pd3dDeviceIndRes, ID3D12GraphicsCommandList *pd3dCommandList)
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

	m_pLights->m_xmf4GlobalAmbient = XMFLOAT4(0.1f, 0.1f, 0.1f, 1.0f);

	m_pLights->m_pLights[0].m_bEnable = true;
	m_pLights->m_pLights[0].m_nType = POINT_LIGHT;
	m_pLights->m_pLights[0].m_fRange = 100.0f;
	m_pLights->m_pLights[0].m_xmf4Ambient = XMFLOAT4(0.1f, 0.0f, 0.0f, 1.0f);
	m_pLights->m_pLights[0].m_xmf4Diffuse = XMFLOAT4(0.8f, 0.0f, 0.0f, 1.0f);
	m_pLights->m_pLights[0].m_xmf4Specular = XMFLOAT4(0.1f, 0.1f, 0.1f, 0.0f);
	m_pLights->m_pLights[0].m_xmf3Position = XMFLOAT3(130.0f, 30.0f, 30.0f);
	m_pLights->m_pLights[0].m_xmf3Direction = XMFLOAT3(0.0f, 0.0f, 0.0f);
	m_pLights->m_pLights[0].m_xmf3Attenuation = XMFLOAT3(1.0f, 0.001f, 0.0001f);
	m_pLights->m_pLights[1].m_bEnable = true;
	m_pLights->m_pLights[1].m_nType = SPOT_LIGHT;
	m_pLights->m_pLights[1].m_fRange = 100.0f;
	m_pLights->m_pLights[1].m_xmf4Ambient = XMFLOAT4(0.1f, 0.1f, 0.1f, 1.0f);
	m_pLights->m_pLights[1].m_xmf4Diffuse = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
	m_pLights->m_pLights[1].m_xmf4Specular = XMFLOAT4(0.1f, 0.1f, 0.1f, 0.0f);
	m_pLights->m_pLights[1].m_xmf3Position = XMFLOAT3(-50.0f, 20.0f, -5.0f);
	m_pLights->m_pLights[1].m_xmf3Direction = XMFLOAT3(0.0f, 0.0f, 1.0f);
	m_pLights->m_pLights[1].m_xmf3Attenuation = XMFLOAT3(1.0f, 0.01f, 0.0001f);
	m_pLights->m_pLights[1].m_fFalloff = 8.0f;
	m_pLights->m_pLights[1].m_fPhi = (float)cos(XMConvertToRadians(40.0f));
	m_pLights->m_pLights[1].m_fTheta = (float)cos(XMConvertToRadians(20.0f));
	m_pLights->m_pLights[2].m_bEnable = true;
	m_pLights->m_pLights[2].m_nType = DIRECTIONAL_LIGHT;
	m_pLights->m_pLights[2].m_xmf4Ambient = XMFLOAT4(0.1f, 0.1f, 0.1f, 1.0f);
	m_pLights->m_pLights[2].m_xmf4Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	m_pLights->m_pLights[2].m_xmf4Specular = XMFLOAT4(1.0f, 1.0f, 1.0f, 0.0f);
	m_pLights->m_pLights[2].m_xmf3Direction = XMFLOAT3(0.0f, -1.0f, 0.0f);
	m_pLights->m_pLights[1].m_xmf3Position = XMFLOAT3(-50.0f, 1020.0f, -5.0f);
	m_pLights->m_pLights[3].m_bEnable = true;
	m_pLights->m_pLights[3].m_nType = SPOT_LIGHT;
	m_pLights->m_pLights[3].m_fRange = 60.0f;
	m_pLights->m_pLights[3].m_xmf4Ambient = XMFLOAT4(0.1f, 0.1f, 0.1f, 1.0f);
	m_pLights->m_pLights[3].m_xmf4Diffuse = XMFLOAT4(0.5f, 0.0f, 0.0f, 1.0f);
	m_pLights->m_pLights[3].m_xmf4Specular = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);
	m_pLights->m_pLights[3].m_xmf3Position = XMFLOAT3(-150.0f, 30.0f, 30.0f);
	m_pLights->m_pLights[3].m_xmf3Direction = XMFLOAT3(0.0f, 1.0f, 1.0f);
	m_pLights->m_pLights[3].m_xmf3Attenuation = XMFLOAT3(1.0f, 0.01f, 0.0001f);
	m_pLights->m_pLights[3].m_fFalloff = 8.0f;
	m_pLights->m_pLights[3].m_fPhi = (float)cos(XMConvertToRadians(90.0f));
	m_pLights->m_pLights[3].m_fTheta = (float)cos(XMConvertToRadians(30.0f));

	m_pMaterials = new MATERIALS;
	::ZeroMemory(m_pMaterials, sizeof(MATERIALS));

	m_pMaterials->m_pReflections[0] = { XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f), XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 5.0f), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f) };
	m_pMaterials->m_pReflections[1] = { XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f), XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 10.0f), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f) };
	m_pMaterials->m_pReflections[2] = { XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f), XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 15.0f), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f) };
	m_pMaterials->m_pReflections[3] = { XMFLOAT4(0.5f, 0.0f, 1.0f, 1.0f), XMFLOAT4(0.0f, 0.5f, 1.0f, 1.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 20.0f), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f) };
	m_pMaterials->m_pReflections[4] = { XMFLOAT4(0.0f, 0.5f, 1.0f, 1.0f), XMFLOAT4(0.5f, 0.0f, 1.0f, 1.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 25.0f), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f) };
	m_pMaterials->m_pReflections[5] = { XMFLOAT4(0.0f, 0.5f, 0.5f, 1.0f), XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 30.0f), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f) };
	m_pMaterials->m_pReflections[6] = { XMFLOAT4(0.5f, 0.5f, 1.0f, 1.0f), XMFLOAT4(0.5f, 0.5f, 1.0f, 1.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 35.0f), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f) };
	m_pMaterials->m_pReflections[7] = { XMFLOAT4(0.0f, 0.8f, 0.0f, 1.0f), XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 40.0f), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f) };
}
void CScene::BuildObjects(CD3DDeviceIndRes *pd3dDeviceIndRes, ID3D12GraphicsCommandList
	*pd3dCommandList)
{
	m_pd3dGraphicsRootSignature = CreateGraphicsRootSignature(pd3dDeviceIndRes);
	//지형을 확대할 스케일 벡터이다. x-축과 z-축은 8배, y-축은 2배 확대한다.
	XMFLOAT3 xmf3Scale(20.0f, 2.0f, 20.0f);
	XMFLOAT4 xmf4Color(0.0f, 0.2f, 0.0f, 0.0f);
	//지형을 높이 맵 이미지 파일(HeightMap.raw)을 사용하여 생성한다. 높이 맵의 크기는 가로x세로(257x257)이다.
#ifdef _WITH_TERRAIN_PARTITION
	/*하나의 격자 메쉬의 크기는 가로x세로(17x17)이다. 지형 전체는 가로 방향으로 16개, 세로 방향으로 16의 격자 메
	쉬를 가진다. 지형을 구성하는 격자 메쉬의 개수는 총 256(16x16)개가 된다.*/
	m_pTerrain = new CHeightMapTerrain(pd3dDeviceIndRes, pd3dCommandList,
		m_pd3dGraphicsRootSignature, _T("Assets/Image/Terrain/HeightMap.raw"), 257, 257, 17,
		17, xmf3Scale, xmf4Color);
#else
	//지형을 하나의 격자 메쉬(257x257)로 생성한다.
	m_pTerrain = new CHeightMapTerrain(pd3dDeviceIndRes, pd3dCommandList,
		m_pd3dGraphicsRootSignature.Get(), _T("Assets/Image/Terrain/HeightMap.raw"), 257, 257, 257,
		257, xmf3Scale, xmf4Color);
#endif
	CCubeMeshDiffused *pRoofMesh = new CCubeMeshDiffused(pd3dDeviceIndRes, pd3dCommandList,
		1100.0f, 50.0f, 1100.0f);
	m_pRoofObject = new CGameObject(1);
	m_pRoofObject->SetObjectType(CGameObject::ObjectType::Wall);
	m_pRoofObject->SetMaterial(rand() % MAX_MATERIALS);
	m_pRoofObject->SetMesh(0, pRoofMesh);
	m_pRoofObject->SetOOBB(pRoofMesh->GetBoundingBox());
	m_pRoofObject->SetPosition(525, m_pTerrain->GetHeight(525, 600) + 60.0f, 600);
	CObjectsShader* pRoofShader = new CObjectsShader();
	pRoofShader->CreateShader(pd3dDeviceIndRes, m_pd3dGraphicsRootSignature.Get());
	CGameObject** container = new CGameObject*[1];
	container[0] = m_pRoofObject;
	pRoofShader->BuildObjects(pd3dDeviceIndRes, pd3dCommandList, container, 1);
	m_pRoofObject->GetMaterial()->SetShader(pRoofShader);

	m_nShaders = ObjectTag::Count;
	m_pShaders = new CInstancingShader[m_nShaders];

	m_pShaders[ObjectTag::Background].CreateShader(pd3dDeviceIndRes, m_pd3dGraphicsRootSignature.Get());
	m_pppObjects = new CGameObject**[ObjectTag::Count];

	m_pnObjects[ObjectTag::Background] = 0;
	m_Maze.m_nPathesPos = 0;
	for (int z = 0; z < m_Maze.m_Length; z++)
	{
		for (int x = 0; x < m_Maze.m_Width; x++)
		{
			switch (m_Maze.m_ppMap[z][x])
			{
			case Maze::Structure::Wall:
				m_pnObjects[ObjectTag::Background]++;
				break;
			case Maze::Structure::Path:
			case Maze::Structure::Entrance:
			case Maze::Structure::Exit:
				m_Maze.m_nPathesPos++;
				break;
			}
		}
	}
	m_pppObjects[ObjectTag::Background] =
		new CGameObject*[m_pnObjects[ObjectTag::Background]];
	m_Maze.m_pxmf3PathesPos = new XMFLOAT3[m_Maze.m_nPathesPos];

	CCubeMeshDiffused *pCubeMesh = new CCubeMeshDiffused(pd3dDeviceIndRes, pd3dCommandList,
		50.0f, 50.0f, 50.0f);
	float fxPitch = 50.0f, fyPitch = 50.0f, fzPitch = 50.0f;
	int i = 0, nPathIdx = 0;
	//XMFLOAT3 xmf3RotateAxis, xmf3SurfaceNormal;
	CRotatingObject *pRotatingObject = NULL;
	i = 0;
	for (int z = 0; z < m_Maze.m_Length; z++)
	{
		for (int x = 0; x < m_Maze.m_Width; x++)
		{
			float xPosition = x * fxPitch + 25.0f;
			float zPosition = z * fzPitch + 100.0f;
			float fHeight = m_pTerrain->GetHeight(xPosition, zPosition);
			switch (m_Maze.m_ppMap[z][x])
			{
			case Maze::Structure::Path:
				m_Maze.m_pxmf3PathesPos[nPathIdx++] = 
					XMFLOAT3(xPosition, fHeight + 25.0f, zPosition);
				break;
			case Maze::Structure::Entrance:
				m_Maze.m_xmf3EntrancePos = 
					XMFLOAT3(xPosition, fHeight + 25.0f, zPosition);
				break;
			case Maze::Structure::Exit:
				m_Maze.m_xmf3ExitPos =
					XMFLOAT3(xPosition, fHeight + 25.0f, zPosition);
				break;
			case Maze::Structure::Wall:
			{
				pRotatingObject = new CRotatingObject(1);
				pRotatingObject->SetObjectType(CGameObject::ObjectType::Wall);
				pRotatingObject->SetMaterial(i % MAX_MATERIALS);
				pRotatingObject->SetMesh(0, pCubeMesh);
				pRotatingObject->SetOOBB(pCubeMesh->GetBoundingBox());
				pRotatingObject->SetPosition(xPosition, fHeight + 25.0f, zPosition);
				pRotatingObject->SetMovingDirection(XMFLOAT3(0, 0, 0));
				pRotatingObject->SetMovingSpeed(0.0f);
				pRotatingObject->SetRotationAxis(XMFLOAT3(0.0f, 1.0f, 0.0f));
				pRotatingObject->SetRotationSpeed(0.0f);
				m_pppObjects[ObjectTag::Background][i++] = pRotatingObject;
				break;
			}
			}
		}
	}
	
	m_pShaders[ObjectTag::Background].BuildObjects(
		pd3dDeviceIndRes, pd3dCommandList,
		m_pppObjects[ObjectTag::Background], m_pnObjects[ObjectTag::Background]);

	m_pShaders[ObjectTag::Objects].CreateShader(pd3dDeviceIndRes, m_pd3dGraphicsRootSignature.Get());
	m_pnObjects[ObjectTag::Objects] = 30;
	m_pppObjects[ObjectTag::Objects] =
		new CGameObject*[m_pnObjects[ObjectTag::Objects]];
	CSphereMeshDiffused *pSphereMeshs =
		new CSphereMeshDiffused(pd3dDeviceIndRes, pd3dCommandList, 6.0f, 20, 20);
	for (i = 0; i < m_pnObjects[ObjectTag::Objects]; ++i)
	{
		CRotatingObject* Sphere = new CRotatingObject();
		int idx = rand() % m_Maze.m_nPathesPos;
		Sphere->SetMaterial(i % MAX_MATERIALS);
		Sphere->SetMesh(0, pSphereMeshs);
		Sphere->SetObjectType(CGameObject::ObjectType::Enemy);
		Sphere->SetMovingDirection(XMFLOAT3(0.0f, 0.0f, 0.0f));
		Sphere->SetPosition(
			m_Maze.m_pxmf3PathesPos[idx].x + (float)RANDOM_NUM(-19, 19),
			m_Maze.m_pxmf3PathesPos[idx].y -19,
			m_Maze.m_pxmf3PathesPos[idx].z + (float)RANDOM_NUM(-19, 19));
		Sphere->SetOOBB(pSphereMeshs->GetBoundingBox());
		Sphere->SetRotationAxis(XMFLOAT3(
			(float)RANDOM_NUM(-1000, 1000),
			(float)RANDOM_NUM(-1000, 1000),
			(float)RANDOM_NUM(-1000, 1000)));
		Sphere->SetRotationSpeed((float)RANDOM_NUM(50, 100));
		Sphere->SetMovingSpeed((float)RANDOM_NUM(10, 20));
		m_pppObjects[ObjectTag::Objects][i] = Sphere;
	}
	m_pShaders[ObjectTag::Objects].BuildObjects(
		pd3dDeviceIndRes, pd3dCommandList,
		m_pppObjects[ObjectTag::Objects], m_pnObjects[ObjectTag::Objects]);
	
	m_pShaders[ObjectTag::Bullet].CreateShader(pd3dDeviceIndRes, m_pd3dGraphicsRootSignature.Get());
	m_pnObjects[ObjectTag::Bullet] = 50;
	m_pppObjects[ObjectTag::Bullet] =
		new CGameObject*[m_pnObjects[ObjectTag::Bullet]];
	CCubeMeshDiffused *pBulleteMeshs =
		new CCubeMeshDiffused(pd3dDeviceIndRes, pd3dCommandList, 2.0f, 2.0f, 2.0f);
	for (i = 0; i < m_pnObjects[ObjectTag::Bullet]; ++i)
	{
		CRotatingObject* Cube = new CRotatingObject();
		Cube->SetObjectType(CGameObject::ObjectType::Bullet);
		Cube->SetMaterial(i % MAX_MATERIALS);
		Cube->SetPosition(0.0f, 100.0f, 0.0f);
		Cube->SetOOBB(pBulleteMeshs->GetBoundingBox());
		Cube->SetRotationAxis(XMFLOAT3(
			(float)RANDOM_NUM(-1000, 1000),
			(float)RANDOM_NUM(-1000, 1000),
			(float)RANDOM_NUM(-1000, 1000)));
		Cube->SetRotationSpeed((float)RANDOM_NUM(50, 100));
		Cube->SetMovingSpeed(100.0f);
		Cube->Sleep();
		m_pppObjects[ObjectTag::Bullet][i] = Cube;
	}
	m_pppObjects[ObjectTag::Bullet][0]->SetMesh(0, pBulleteMeshs);
	m_pShaders[ObjectTag::Bullet].BuildObjects(
		pd3dDeviceIndRes, pd3dCommandList,
		m_pppObjects[ObjectTag::Bullet], m_pnObjects[ObjectTag::Bullet]);
	
	m_pShaders[ObjectTag::Particle].CreateShader(pd3dDeviceIndRes, m_pd3dGraphicsRootSignature.Get());
	m_pnObjects[ObjectTag::Particle] = 500;
	m_pppObjects[ObjectTag::Particle] =
		new CGameObject*[m_pnObjects[ObjectTag::Particle]];
	CCubeMeshDiffused *pParticleMeshs =
		new CCubeMeshDiffused(pd3dDeviceIndRes, pd3dCommandList, 1.0f, 1.0f, 1.0f);
	for (i = 0; i < m_pnObjects[ObjectTag::Particle]; ++i)
	{
		CRotatingObject* Cube = new CRotatingObject();
		Cube->SetObjectType(CGameObject::ObjectType::Particle);
		Cube->SetMaterial(i % MAX_MATERIALS);
		Cube->SetPosition(0.0f, 100.0f, 0.0f);
		Cube->SetOOBB(pParticleMeshs->GetBoundingBox());
		Cube->SetRotationAxis(XMFLOAT3(
			(float)RANDOM_NUM(-1000, 1000),
			(float)RANDOM_NUM(-1000, 1000),
			(float)RANDOM_NUM(-1000, 1000)));
		Cube->SetRotationSpeed((float)RANDOM_NUM(50, 100));
		Cube->SetMovingSpeed(100.0f);
		Cube->Sleep();
		m_pppObjects[ObjectTag::Particle][i] = Cube;
	}
	m_pppObjects[ObjectTag::Particle][0]->SetMesh(0, pParticleMeshs);
	m_pShaders[ObjectTag::Particle].BuildObjects(
		pd3dDeviceIndRes, pd3dCommandList,
		m_pppObjects[ObjectTag::Particle], m_pnObjects[ObjectTag::Particle]);

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

	if (m_pTerrain) m_pTerrain->Render(pd3dCommandList, pCamera);
	for (int i = 0; i < m_nShaders; i++)
	{
		m_pShaders[i].Render(pd3dCommandList, pCamera);
	}
	if (m_pRoofObject) m_pRoofObject->Render(pd3dCommandList, pCamera);
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
		if (pKeyBuffer['R'] & 0xF0) dwDirection |= DIR_UP;
		if (pKeyBuffer['F'] & 0xF0) dwDirection |= DIR_DOWN;
		if (pKeyBuffer['P'] & 0xF0)
			m_pPlayer->SetPosition(m_Maze.m_xmf3EntrancePos);
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
		m_pPlayer->Sleep();
		m_fRestartCounter += fTimeElapsed;
		if (m_fRestartCounter > 2)
		{
			m_fRestartCounter = 0.0f;
			m_bGameOver = false;
			m_pPlayer->Awake();
			m_pPlayer->Reset();
			m_pPlayer->SetPosition(m_Maze.m_xmf3EntrancePos);
		}
	}
	m_fBulletTimer += fTimeElapsed;
	for (int i = 0; i < m_nShaders; i++)
	{
		m_pShaders[i].AnimateObjects(fTimeElapsed);
	}
	m_pPlayer->Animate(fTimeElapsed);
	//PhysicsProcessing(fTimeElapsed);

	for (int i = 0; i < m_pnObjects[ObjectTag::Bullet]; ++i)
	{
		CRotatingObject* Object = dynamic_cast<CRotatingObject*>(
			m_pppObjects[ObjectTag::Bullet][i]);
		XMFLOAT3 BulletPos = Object->GetPosition();
		if (BulletPos.y < m_pTerrain->GetHeight(BulletPos.x, BulletPos.z))
		{
			Object->SetPosition(0.0f, 1000.0f, 0.0f);
			Object->Animate(fTimeElapsed);
			Object->Sleep();
		}
	}

	for (int i = 0; i < m_pnObjects[ObjectTag::Objects]; ++i)
	{
		CRotatingObject* Object = dynamic_cast<CRotatingObject*>(
			m_pppObjects[ObjectTag::Objects][i]);
		XMFLOAT3 ObjectPos = Object->GetPosition();
		Object->SetPosition(ObjectPos.x, 
			m_pTerrain->GetHeight(ObjectPos.x, ObjectPos.z) + 6.0f, 
			ObjectPos.z);
		XMFLOAT3 direct = Vector3::Add(PlayerPos, ObjectPos, -1.0f);
		if (XMVector3Length(XMLoadFloat3(&direct)).m128_f32[0] < 150.0f)
		{
			XMFLOAT3 look = Vector3::Normalize(direct);
			Object->SetMovingDirection(look);
		}
		else
		{
			Object->SetMovingDirection(XMFLOAT3(0.0f, 0.0f, 0.0f));
		}
	}

	for (int i = 0; i < m_pnObjects[ObjectTag::Particle]; ++i)
	{
		CRotatingObject* Particle = dynamic_cast<CRotatingObject*>(
			m_pppObjects[ObjectTag::Particle][i]);
		Particle->GetMovingRange() -= fTimeElapsed;
		if (Particle->GetMovingRange() < 0)
		{
			Particle->SetPosition(0.0f, 1000.0f, 0.0f);
			Particle->Animate(fTimeElapsed);
			Particle->Sleep();
		}
	}
	if (m_pLights)
	{
		m_pLights->m_pLights[1].m_xmf3Position = m_pPlayer->GetPosition();
		m_pLights->m_pLights[1].m_xmf3Direction = m_pPlayer->GetLookVector();
	}
}

void CScene::PhysicsProcessing(float fTimeElapsed)
{
	// Collision check between walls and all objects
	for (int i = 0; i < m_pnObjects[ObjectTag::Background]; ++i)
	{
		CRotatingObject* Wall = dynamic_cast<CRotatingObject*>(
			m_pppObjects[ObjectTag::Background][i]);
		BoundingOrientedBox wmWallOOBB = Wall->GetOOBB();
		XMFLOAT3 corners[8];
		wmWallOOBB.GetCorners(corners);
		XMFLOAT4 pxmf4WallPlanes[4];
		XMStoreFloat4(&pxmf4WallPlanes[0], XMPlaneNormalize(XMPlaneFromPoints(
			XMLoadFloat3(&corners[0]),
			XMLoadFloat3(&corners[1]),
			XMLoadFloat3(&corners[2]))));
		XMStoreFloat4(&pxmf4WallPlanes[1], XMPlaneNormalize(XMPlaneFromPoints(
			XMLoadFloat3(&corners[6]),
			XMLoadFloat3(&corners[5]),
			XMLoadFloat3(&corners[8]))));
		XMStoreFloat4(&pxmf4WallPlanes[2], XMPlaneNormalize(XMPlaneFromPoints(
			XMLoadFloat3(&corners[5]),
			XMLoadFloat3(&corners[1]),
			XMLoadFloat3(&corners[4]))));
		XMStoreFloat4(&pxmf4WallPlanes[3], XMPlaneNormalize(XMPlaneFromPoints(
			XMLoadFloat3(&corners[2]),
			XMLoadFloat3(&corners[6]),
			XMLoadFloat3(&corners[7]))));

		for (int j = 0; j < m_pnObjects[ObjectTag::Bullet]; ++j)
		{
			if (!m_pppObjects[ObjectTag::Bullet][j]->IsActive()) continue;
			BoundingOrientedBox wmBulletOOBB =
				m_pppObjects[ObjectTag::Bullet][j]->GetOOBB();
			ContainmentType containType = wmWallOOBB.Contains(wmBulletOOBB);
			switch (containType)
			{
			case CONTAINS:
			{
				m_pppObjects[ObjectTag::Bullet][j]->SetPosition(0.0f, 1000.0f, 0.0f);
				m_pppObjects[ObjectTag::Bullet][j]->Animate(fTimeElapsed);
				m_pppObjects[ObjectTag::Bullet][j]->Sleep();
				break;
			}
			case INTERSECTS:
			{
				m_pppObjects[ObjectTag::Bullet][j]->SetPosition(0.0f, 1000.0f, 0.0f);
				m_pppObjects[ObjectTag::Bullet][j]->Animate(fTimeElapsed);
				m_pppObjects[ObjectTag::Bullet][j]->Sleep();
				break;
			}
			default:
				break;
			}
		}
		for (int j = 0; j < m_pnObjects[ObjectTag::Particle]; ++j)
		{
			if (!m_pppObjects[ObjectTag::Particle][j]->IsActive()) continue;
			BoundingOrientedBox wmParticleOOBB =
				m_pppObjects[ObjectTag::Particle][j]->GetOOBB();
			ContainmentType containType = wmWallOOBB.Contains(wmParticleOOBB);
			switch (containType)
			{
			case CONTAINS:
			{
				CRotatingObject* Particle = dynamic_cast<CRotatingObject*>(
					m_pppObjects[ObjectTag::Particle][j]);
				int plane_idx = -1;
				float dot = 1;
				for (int k = 0; k < 4; k++)
				{
					XMVECTOR xmvDot = XMPlaneDotNormal(XMLoadFloat4(&pxmf4WallPlanes[k]),
						XMLoadFloat3(&Particle->GetMovingDirection()));
					if (dot > xmvDot.m128_f32[0])
					{
						dot = xmvDot.m128_f32[0];
						plane_idx = k;
					}
				}
				XMVECTOR xmvNormal = XMVectorSet(
					pxmf4WallPlanes[plane_idx].x,
					pxmf4WallPlanes[plane_idx].y,
					pxmf4WallPlanes[plane_idx].z, 0.0f);
				XMVECTOR xmvReflect = XMVector3Reflect(XMLoadFloat3(
					&Particle->GetMovingDirection()), xmvNormal);
				XMStoreFloat3(&Particle->GetMovingDirection(), XMLoadFloat3(
					&Vector3::Normalize(Vector3::XMVectorToFloat3(xmvReflect))));
				Particle->Move(Vector3::Normalize(
					Vector3::Add(XMFLOAT3(
						pxmf4WallPlanes[plane_idx].x,
						pxmf4WallPlanes[plane_idx].y,
						pxmf4WallPlanes[plane_idx].z),
						Vector3::XMVectorToFloat3(xmvReflect))),
					REPULSIVE_FORCE * fTimeElapsed);
				break;
			}
			case INTERSECTS:
			{
				CRotatingObject* Particle = dynamic_cast<CRotatingObject*>(
					m_pppObjects[ObjectTag::Particle][j]);
				int plane_idx = -1;
				float dot = 1;
				for (int k = 0; k < 4; k++)
				{
					XMVECTOR xmvDot = XMPlaneDotNormal(XMLoadFloat4(&pxmf4WallPlanes[k]),
						XMLoadFloat3(&Particle->GetMovingDirection()));
					if (dot > xmvDot.m128_f32[0])
					{
						dot = xmvDot.m128_f32[0];
						plane_idx = k;
					}
				}
				XMVECTOR xmvNormal = XMVectorSet(
					pxmf4WallPlanes[plane_idx].x,
					pxmf4WallPlanes[plane_idx].y,
					pxmf4WallPlanes[plane_idx].z, 0.0f);
				XMVECTOR xmvReflect = XMVector3Reflect(XMLoadFloat3(
					&Particle->GetMovingDirection()), xmvNormal);
				XMStoreFloat3(&Particle->GetMovingDirection(), XMLoadFloat3(
					&Vector3::Normalize(Vector3::XMVectorToFloat3(xmvReflect))));
				Particle->Move(Vector3::Normalize(
					Vector3::Add(XMFLOAT3(
						pxmf4WallPlanes[plane_idx].x,
						pxmf4WallPlanes[plane_idx].y,
						pxmf4WallPlanes[plane_idx].z),
						Vector3::XMVectorToFloat3(xmvReflect))),
					REPULSIVE_FORCE * fTimeElapsed);
				break;
			}
			}
		}
		for (int j = 0; j < m_pnObjects[ObjectTag::Objects]; ++j)
		{
			BoundingOrientedBox wmObjectOOBB =
				m_pppObjects[ObjectTag::Objects][j]->GetOOBB();
			ContainmentType containType = wmWallOOBB.Contains(wmObjectOOBB);
			switch (containType)
			{
			case CONTAINS:
			{
				CRotatingObject* Object = dynamic_cast<CRotatingObject*>(
					m_pppObjects[ObjectTag::Objects][j]);
				Object->Move(Vector3::Normalize(Vector3::Add(
					Object->GetPosition(), Wall->GetPosition(), -1)),
					REPULSIVE_FORCE * fTimeElapsed);
				break;
			}
			case INTERSECTS:
			{
				CRotatingObject* Object = dynamic_cast<CRotatingObject*>(
					m_pppObjects[ObjectTag::Objects][j]);
				Object->Move(Vector3::Normalize(Vector3::Add(
					Object->GetPosition(), Wall->GetPosition(), -1)),
					REPULSIVE_FORCE * fTimeElapsed);
				break;
			}
			}
		}
		BoundingOrientedBox wmPlayerOOBB = m_pPlayer->GetOOBB();
		ContainmentType containType = wmWallOOBB.Contains(wmPlayerOOBB);
		switch (containType)
		{
		case CONTAINS:
		{
			m_pPlayer->Move(Vector3::ScalarProduct(Vector3::Add(
				m_pPlayer->GetPosition(), Wall->GetPosition(), -1),
				2 * REPULSIVE_FORCE * fTimeElapsed), false);
			break;
		}
		case INTERSECTS:
		{
			m_pPlayer->Move(Vector3::ScalarProduct(Vector3::Add(
				m_pPlayer->GetPosition(), Wall->GetPosition(), -1),
				2 * REPULSIVE_FORCE * fTimeElapsed), false);
			break;
		}
		}
	}

	// Collision check between all objects
	for (int i = 0; i < m_pnObjects[ObjectTag::Objects]; ++i)
	{
		m_pppObjects[ObjectTag::Objects][i]->SetCollider(NULL);
	}
	for (int i = 0; i < m_pnObjects[ObjectTag::Objects]; ++i)
	{
		BoundingOrientedBox wmObjectOOBB1 =
			m_pppObjects[ObjectTag::Objects][i]->GetOOBB();
		for (int j = i + 1; j < m_pnObjects[ObjectTag::Objects]; ++j)
		{
			BoundingOrientedBox wmObjectOOBB2 =
				m_pppObjects[ObjectTag::Objects][j]->GetOOBB();
			if (wmObjectOOBB1.Intersects(wmObjectOOBB2))
			{
				m_pppObjects[ObjectTag::Objects][i]->SetCollider(
					m_pppObjects[ObjectTag::Objects][j]);
				m_pppObjects[ObjectTag::Objects][j]->SetCollider(
					m_pppObjects[ObjectTag::Objects][i]);
			}
		}
		if (!m_bGameOver && m_pPlayer->GetOOBB().Intersects(wmObjectOOBB1))
		{
			if (m_pPlayer->GetCamera()->GetMode() == THIRD_PERSON_CAMERA)
			{
				PopParticles(m_pPlayer->GetPosition(), 300);
				m_bGameOver = true;
			}
		}
	}
	for (int i = 0; i < m_pnObjects[ObjectTag::Objects]; ++i)
	{
		CRotatingObject* Object = dynamic_cast<CRotatingObject*>(
			m_pppObjects[ObjectTag::Objects][i]);
		if (Object->GetCollider() && Object->GetCollider()->GetCollider())
		{
			CRotatingObject* Collider = dynamic_cast<CRotatingObject*>(
				Object->GetCollider());
			float fMovingSpeed = Object->GetMovingSpeed();
			XMFLOAT3 xmf3ObjPos = Object->GetPosition();
			XMFLOAT3 xmf3ColliderPos = Collider->GetPosition();

			XMFLOAT3 direct = Vector3::Normalize(Vector3::Add(
				xmf3ObjPos, xmf3ColliderPos, -1.0f));

			Object->SetMovingSpeed(Collider->GetMovingSpeed());
			Object->Move(direct, REPULSIVE_FORCE * fTimeElapsed);

			direct = Vector3::Normalize(Vector3::Add(
				xmf3ColliderPos, xmf3ObjPos, -1.0f));
			
			Collider->SetMovingSpeed(fMovingSpeed);
			Collider->Move(direct, REPULSIVE_FORCE * fTimeElapsed);

			Object->SetCollider(NULL);
			Collider->SetCollider(NULL);
		}
	}

	for (int i = 0; i < m_pnObjects[ObjectTag::Bullet]; ++i)
	{
		if (!m_pppObjects[ObjectTag::Bullet][i]->IsActive()) continue;
		BoundingOrientedBox wmBulletOOBB =
			m_pppObjects[ObjectTag::Bullet][i]->GetOOBB();
		for (int j = 0; j < m_pnObjects[ObjectTag::Objects]; ++j)
		{
			BoundingOrientedBox wmObjectOOBB =
				m_pppObjects[ObjectTag::Objects][j]->GetOOBB();
			if (wmBulletOOBB.Intersects(wmObjectOOBB))
			{
				CRotatingObject* Object = dynamic_cast<CRotatingObject*>(
					m_pppObjects[ObjectTag::Objects][j]);
				if(m_pSelectedObject == Object)
					m_pSelectedObject = nullptr;
				PopParticles(Object->GetPosition());
				ResetObjects(Object);
				m_pppObjects[ObjectTag::Bullet][i]->SetPosition(0.0f, 1000.0f, 0.0f);
				m_pppObjects[ObjectTag::Bullet][i]->Animate(fTimeElapsed);
				m_pppObjects[ObjectTag::Bullet][i]->Sleep();
			}
		}
	}
}

void CScene::ReleaseUploadBuffers()
{
	for (int i = 0; i < m_nShaders; i++) m_pShaders[i].ReleaseUploadBuffers();
	if (m_pTerrain) m_pTerrain->ReleaseUploadBuffers();
}

bool CScene::OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM
	lParam)
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

CGameObject *CScene::PickObjectPointedByCursor(int xClient, int yClient, CCamera
	*pCamera)
{
	if (!pCamera) return(NULL);
	XMFLOAT4X4 xmf4x4View = pCamera->GetViewMatrix();
	XMFLOAT4X4 xmf4x4Projection = pCamera->GetProjectionMatrix();
	D3D12_VIEWPORT d3dViewport = pCamera->GetViewport();
	XMFLOAT3 xmf3PickPosition;
	/*화면 좌표계의 점 (xClient, yClient)를 화면 좌표 변환의 역변환과 투영 변환의 역변환을 한다. 그 결과는 카메라
	좌표계의 점이다. 투영 평면이 카메라에서 z-축으로 거리가 1이므로 z-좌표는 1로 설정한다.*/
	xmf3PickPosition.x = (((2.0f * xClient) / d3dViewport.Width) - 1) /
		xmf4x4Projection._11;
	xmf3PickPosition.y = -(((2.0f * yClient) / d3dViewport.Height) - 1) /
		xmf4x4Projection._22;
	xmf3PickPosition.z = 1.0f;
	int nIntersected = 0;
	float fHitDistance = FLT_MAX, fNearestHitDistance = FLT_MAX;
	CGameObject *pIntersectedObject = NULL, *pNearestObject = NULL;
	//셰이더의 모든 게임 객체들에 대한 마우스 픽킹을 수행하여 카메라와 가장 가까운 게임 객체를 구한다.
	//for (int i = 0; i < m_nShaders; i++)
	{
		pIntersectedObject = m_pShaders[ObjectTag::Objects].PickObjectByRayIntersection(xmf3PickPosition,
			xmf4x4View, &fHitDistance);
		if (pIntersectedObject && (fHitDistance < fNearestHitDistance))
		{
			fNearestHitDistance = fHitDistance;
			pNearestObject = pIntersectedObject;
		}
	}
	return(pNearestObject);
}

void CScene::ShootBullet()
{
	if (m_fBulletTimer > BulletDelay)
	{
		m_fBulletTimer = 0.0f;
		if (m_nCurrentBulletIdx == m_pnObjects[ObjectTag::Bullet])
			m_nCurrentBulletIdx = 0;
		CRotatingObject *bullet = dynamic_cast<CRotatingObject*>(
			m_pppObjects[ObjectTag::Bullet][m_nCurrentBulletIdx++]);
		bullet->Awake();
		if (m_pSelectedObject)
		{
			bullet->SetMovingDirection(
				Vector3::Normalize(Vector3::Add(
					m_pSelectedObject->GetPosition(),
					m_pPlayer->GetPosition(), -1.0f)));
		}
		else
		{
			bullet->SetMovingDirection(m_pPlayer->GetLook());
		}
		bullet->SetPosition(m_pPlayer->GetPosition());
		m_pSelectedObject = NULL;
	}
}

void CScene::ResetObjects(CRotatingObject * Object)
{
	int idx = rand() % m_Maze.m_nPathesPos;
	Object->SetObjectType(CGameObject::ObjectType::Enemy);
	Object->SetMovingDirection(XMFLOAT3(0.0f, 0.0f, 0.0f));
	Object->SetPosition(
		m_Maze.m_pxmf3PathesPos[idx].x + (float)RANDOM_NUM(-19, 19),
		m_Maze.m_pxmf3PathesPos[idx].y - 19,
		m_Maze.m_pxmf3PathesPos[idx].z + (float)RANDOM_NUM(-19, 19));
	Object->SetRotationAxis(XMFLOAT3(
		(float)RANDOM_NUM(-1000, 1000),
		(float)RANDOM_NUM(-1000, 1000),
		(float)RANDOM_NUM(-1000, 1000)));
	Object->SetRotationSpeed((float)RANDOM_NUM(50, 100));
	Object->SetMovingSpeed((float)RANDOM_NUM(30, 50));
}

void CScene::PopParticles(XMFLOAT3& pos, int nParticles)
{
	for (int i = 0; i < nParticles; ++i)
	{
		if (m_nCurrentParticleIdx == m_pnObjects[ObjectTag::Particle])
			m_nCurrentParticleIdx = 0;
		CRotatingObject *particle = dynamic_cast<CRotatingObject*>(
			m_pppObjects[ObjectTag::Particle][m_nCurrentParticleIdx++]);
		particle->Awake();
		particle->SetMovingDirection(Vector3::Normalize(XMFLOAT3(
			(float)RANDOM_NUM(-1000, 1000),
			(float)RANDOM_NUM(-1000, 1000),
			(float)RANDOM_NUM(-1000, 1000))));
		particle->SetMovingRange(1.0f);
		particle->SetPosition(pos);
		particle->SetRotationAxis(XMFLOAT3(
			(float)RANDOM_NUM(-1000, 1000),
			(float)RANDOM_NUM(-1000, 1000),
			(float)RANDOM_NUM(-1000, 1000)));
		particle->SetRotationSpeed((float)RANDOM_NUM(50, 100));
		particle->SetMovingSpeed(40.0f);
	}
}

