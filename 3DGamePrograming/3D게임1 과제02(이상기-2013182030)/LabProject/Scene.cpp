#include "stdafx.h"
#include "GameFramework.h"
#include "GameTimer.h"
#include "Camera.h"
#include "Player.h"
#include "Scene.h"


CScene::CScene()
{
	m_pd3dGraphicsRootSignature = NULL;
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
	return(m_pd3dGraphicsRootSignature);
}
ID3D12RootSignature* CScene::CreateGraphicsRootSignature(ID3D12Device *pd3dDevice)
{
	ID3D12RootSignature *pd3dGraphicsRootSignature = NULL;
	D3D12_ROOT_PARAMETER pd3dRootParameters[3];
	pd3dRootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS;
	pd3dRootParameters[0].Constants.Num32BitValues = 16;
	pd3dRootParameters[0].Constants.ShaderRegister = 0; //b0: Player
	pd3dRootParameters[0].Constants.RegisterSpace = 0;
	pd3dRootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;
	pd3dRootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS;
	pd3dRootParameters[1].Constants.Num32BitValues = 32;
	pd3dRootParameters[1].Constants.ShaderRegister = 1; //b1: Camera
	pd3dRootParameters[1].Constants.RegisterSpace = 0;
	pd3dRootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;
	pd3dRootParameters[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_SRV;
	pd3dRootParameters[2].Descriptor.ShaderRegister = 0; //t0
	pd3dRootParameters[2].Descriptor.RegisterSpace = 0;
	pd3dRootParameters[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;

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

	ID3DBlob *pd3dSignatureBlob = NULL;
	ID3DBlob *pd3dErrorBlob = NULL;
	D3D12SerializeRootSignature(&d3dRootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1,
		&pd3dSignatureBlob, &pd3dErrorBlob);

	pd3dDevice->CreateRootSignature(0, pd3dSignatureBlob->GetBufferPointer(),
		pd3dSignatureBlob->GetBufferSize(), __uuidof(ID3D12RootSignature), 
		(void**)&pd3dGraphicsRootSignature);

	if (pd3dSignatureBlob) pd3dSignatureBlob->Release();
	if (pd3dErrorBlob) pd3dErrorBlob->Release();

	return(pd3dGraphicsRootSignature);
}

void CScene::BuildObjects(
	ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList)
{
	m_pd3dGraphicsRootSignature = CreateGraphicsRootSignature(pd3dDevice);
	m_nShaders = ObjectTag::Count;
	m_pShaders = new CInstancingShader[m_nShaders];

	m_pShaders[ObjectTag::Background].CreateShader(pd3dDevice, m_pd3dGraphicsRootSignature);
	m_pppObjects = new CGameObject**[ObjectTag::Count];
	m_pnObjects[ObjectTag::Background] = 49;
	m_pppObjects[ObjectTag::Background] = 
		new CGameObject*[m_pnObjects[ObjectTag::Background]];
	CWallMeshDiffused *pWallMesh = new CWallMeshDiffused(
		pd3dDevice, pd3dCommandList, 40.0f, 40.0f, 10.0f);
	for (int i = 0; i < m_pnObjects[ObjectTag::Background]; ++i)
	{
		CGameObject* Wall = new CGameObject();
		Wall->SetPosition(0.0f, 0.0f, 10.0f * (i - 24));
		Wall->SetOOBB(pWallMesh->GetBoundingBox());
		Wall->SetObjectType(CGameObject::ObjectType::Wall);
		m_pppObjects[ObjectTag::Background][i] = Wall;
	}
	m_pppObjects[ObjectTag::Background][0]->SetMesh(pWallMesh);
	m_pxmf4WallPlanes[0] = XMFLOAT4A(+1.0f, 0.0f, 0.0f, 20.0f);
	m_pxmf4WallPlanes[1] = XMFLOAT4A(-1.0f, 0.0f, 0.0f, 20.0f);
	m_pxmf4WallPlanes[2] = XMFLOAT4A(0.0f, +1.0f, 0.0f, 20.0f);
	m_pxmf4WallPlanes[3] = XMFLOAT4A(0.0f, -1.0f, 0.0f, 20.0f);
	m_pxmf4WallPlanes[4] = XMFLOAT4A(0.0f, 0.0f, +1.0f, 5.0f);
	m_pxmf4WallPlanes[5] = XMFLOAT4A(0.0f, 0.0f, -1.0f, 5.0f);
	m_pShaders[ObjectTag::Background].BuildObjects(
		pd3dDevice, pd3dCommandList,
		m_pppObjects[ObjectTag::Background], m_pnObjects[ObjectTag::Background]);

	m_pShaders[ObjectTag::Objects].CreateShader(pd3dDevice, m_pd3dGraphicsRootSignature);
	m_pnObjects[ObjectTag::Objects] = 20;
	m_pppObjects[ObjectTag::Objects] =
		new CGameObject*[m_pnObjects[ObjectTag::Objects]];
	CCubeMeshDiffused *pCubeMeshs = 
		new CCubeMeshDiffused(pd3dDevice, pd3dCommandList, 4.0f, 4.0f, 4.0f);
	for (int i = 0; i < m_pnObjects[ObjectTag::Objects]; ++i)
	{
		CRotatingObject* Cube = new CRotatingObject();
		switch (rand() % 4)
		{
		case 0:
			Cube->SetObjectType(CGameObject::ObjectType::RedCube);
			Cube->SetMovingDirection(XMFLOAT3(1.0f, 0.0f, 0.0f));
			break;
		case 1:
			Cube->SetObjectType(CGameObject::ObjectType::GreenCube);
			Cube->SetMovingDirection(XMFLOAT3(0.0f, 1.0f, 0.0f));
			break;
		case 2:
			Cube->SetObjectType(CGameObject::ObjectType::BlueCube);
			Cube->SetMovingDirection(XMFLOAT3(0.0f, 0.0f, -1.0f));
			break;
		case 3:
			Cube->SetObjectType(CGameObject::ObjectType::WhiteCube);
			Cube->SetMovingDirection(XMFLOAT3(
				(float)RANDOM_NUM(-1000, 1000),
				(float)RANDOM_NUM(-1000, 1000),
				(float)RANDOM_NUM(-1000, 1000)));
			break;
		}
		Cube->SetPosition(
			(float)RANDOM_NUM(-15, 15),
			(float)RANDOM_NUM(-15, 15),
			(float)RANDOM_NUM(100, 240));
		Cube->SetOOBB(pCubeMeshs->GetBoundingBox());
		Cube->SetRotationAxis(XMFLOAT3(
			(float)RANDOM_NUM(-1000, 1000),
			(float)RANDOM_NUM(-1000, 1000),
			(float)RANDOM_NUM(-1000, 1000)));
		Cube->SetRotationSpeed((float)RANDOM_NUM(50, 100));
		Cube->SetMovingSpeed((float)RANDOM_NUM(10, 20));
		m_pppObjects[ObjectTag::Objects][i] = Cube;
	}
	m_pppObjects[ObjectTag::Objects][0]->SetMesh(pCubeMeshs);
	m_pShaders[ObjectTag::Objects].BuildObjects(
		pd3dDevice, pd3dCommandList,
		m_pppObjects[ObjectTag::Objects], m_pnObjects[ObjectTag::Objects]);

	m_pShaders[ObjectTag::Bullet].CreateShader(pd3dDevice, m_pd3dGraphicsRootSignature);
	m_pnObjects[ObjectTag::Bullet] = 50;
	m_pppObjects[ObjectTag::Bullet] =
		new CGameObject*[m_pnObjects[ObjectTag::Bullet]];
	CCubeMeshDiffused *pBulleteMeshs =
		new CCubeMeshDiffused(pd3dDevice, pd3dCommandList, 2.0f, 2.0f, 2.0f);
	for (int i = 0; i < m_pnObjects[ObjectTag::Bullet]; ++i)
	{
		CRotatingObject* Cube = new CRotatingObject();
		Cube->SetObjectType(CGameObject::ObjectType::Bullet);
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
	m_pppObjects[ObjectTag::Bullet][0]->SetMesh(pBulleteMeshs);
	m_pShaders[ObjectTag::Bullet].BuildObjects(
		pd3dDevice, pd3dCommandList,
		m_pppObjects[ObjectTag::Bullet], m_pnObjects[ObjectTag::Bullet]);
	
	m_pShaders[ObjectTag::Particle].CreateShader(pd3dDevice, m_pd3dGraphicsRootSignature);
	m_pnObjects[ObjectTag::Particle] = 500;
	m_pppObjects[ObjectTag::Particle] =
		new CGameObject*[m_pnObjects[ObjectTag::Particle]];
	CCubeMeshDiffused *pParticleMeshs =
		new CCubeMeshDiffused(pd3dDevice, pd3dCommandList, 1.0f, 1.0f, 1.0f);
	for (int i = 0; i < m_pnObjects[ObjectTag::Particle]; ++i)
	{
		CRotatingObject* Cube = new CRotatingObject();
		Cube->SetObjectType(CGameObject::ObjectType::Particle);
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
	m_pppObjects[ObjectTag::Particle][0]->SetMesh(pParticleMeshs);
	m_pShaders[ObjectTag::Particle].BuildObjects(
		pd3dDevice, pd3dCommandList,
		m_pppObjects[ObjectTag::Particle], m_pnObjects[ObjectTag::Particle]);
}
void CScene::ReleaseObjects()
{
	if (m_pd3dGraphicsRootSignature) m_pd3dGraphicsRootSignature->Release();
	for (int i = 0; i < m_nShaders; i++)
	{
		m_pShaders[i].ReleaseShaderVariables();
		m_pShaders[i].ReleaseObjects();
	}
	if (m_pShaders) delete[] m_pShaders;
}

void CScene::Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera)
{
	pCamera->SetViewportsAndScissorRects(pd3dCommandList);
	pd3dCommandList->SetGraphicsRootSignature(m_pd3dGraphicsRootSignature);
	pCamera->UpdateShaderVariables(pd3dCommandList);
	for (int i = 0; i < m_nShaders; i++)
	{
		m_pShaders[i].Render(pd3dCommandList, pCamera);
	}
}
bool CScene::ProcessInput(float fTimeElapsed)
{
	if (m_bGameOver) return false;
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
		if (pKeyBuffer[VK_SPACE] & 0xF0)
			ShootBullet();
		if (dwDirection)
			m_pPlayer->Move(dwDirection, 50.0f * fTimeElapsed, true);
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

		if (cxDelta || cyDelta)
		{
			if (pKeyBuffer[VK_RBUTTON] & 0xF0)
				m_pPlayer->Rotate(cyDelta, 0.0f, -cxDelta);
			else
				m_pPlayer->Rotate(cyDelta, cxDelta, 0.0f);
		}
	}
	m_pPlayer->Update(fTimeElapsed);
	return true;
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

			for (int i = 0; i < m_pnObjects[ObjectTag::Objects]; ++i)
			{
				CRotatingObject* Object = dynamic_cast<CRotatingObject*>(
					m_pppObjects[ObjectTag::Objects][i]);
				if (Object->GetPosition().z > PlayerPos.z - 100.0f &&
					Object->GetPosition().z < PlayerPos.z + 100.0f)
				{
					ResetObjects(Object, PlayerPos.z);
				}
			}
		}
	}
	m_fBulletTimer += fTimeElapsed;
	for (int i = 0; i < m_nShaders; i++)
	{
		m_pShaders[i].AnimateObjects(fTimeElapsed);
	}
	m_pPlayer->Animate(fTimeElapsed);
	PhysicsProcessing(fTimeElapsed);


	for (int i = 0; i < m_pnObjects[ObjectTag::Background]; ++i)
	{
		CGameObject* Wall = m_pppObjects[ObjectTag::Background][i];
		if (Wall->GetPosition().z < PlayerPos.z - 240.0f)
		{
			Wall->MoveForward(490.0f);
		}
	}

	for (int i = 0; i < m_pnObjects[ObjectTag::Bullet]; ++i)
	{
		CRotatingObject* Object = dynamic_cast<CRotatingObject*>(
			m_pppObjects[ObjectTag::Bullet][i]);
		XMFLOAT3 BulletPos = Object->GetPosition();
		if (BulletPos.x > 20.0f ||
			BulletPos.x < -20.0f ||
			BulletPos.y > 20.0f ||
			BulletPos.y < -20.0f ||
			BulletPos.z < PlayerPos.z - 240.0f ||
			BulletPos.z > PlayerPos.z + 240.0f)
		{
			Object->SetPosition(0.0f, 100.0f, 0.0f);
			Object->Animate(fTimeElapsed);
			Object->Sleep();
		}
	}

	for (int i = 0; i < m_pnObjects[ObjectTag::Objects]; ++i)
	{
		CRotatingObject* Object = dynamic_cast<CRotatingObject*>(
			m_pppObjects[ObjectTag::Objects][i]);
		XMFLOAT3 ObjectPos = Object->GetPosition();
		if (ObjectPos.x > 20.0f ||
			ObjectPos.x < -20.0f ||
			ObjectPos.y > 20.0f ||
			ObjectPos.y < -20.0f ||
			ObjectPos.z < PlayerPos.z - 240.0f ||
			ObjectPos.z > PlayerPos.z + 240.0f)
		{
			ResetObjects(Object, PlayerPos.z);
		}
	}

	for (int i = 0; i < m_pnObjects[ObjectTag::Particle]; ++i)
	{
		CRotatingObject* Particle = dynamic_cast<CRotatingObject*>(
			m_pppObjects[ObjectTag::Particle][i]);
		Particle->GetMovingRange() -= fTimeElapsed;
		if (Particle->GetMovingRange() < 0)
		{
			Particle->SetPosition(0.0f, 100.0f, 0.0f);
			Particle->Animate(fTimeElapsed);
			Particle->Sleep();
		}
	}
}

void CScene::PhysicsProcessing(float fTimeElapsed)
{
	// Collision check between walls and all objects
	for (int i = 0; i < m_pnObjects[ObjectTag::Background]; ++i)
	{
		BoundingOrientedBox wmWallOOBB =
			m_pppObjects[ObjectTag::Background][i]->GetOOBB();
		for (int j = 0; j < m_pnObjects[ObjectTag::Bullet]; ++j)
		{
			if (!m_pppObjects[ObjectTag::Bullet][j]->IsActive()) continue;
			BoundingOrientedBox wmBulletOOBB =
				m_pppObjects[ObjectTag::Bullet][j]->GetOOBB();
			ContainmentType containType = wmWallOOBB.Contains(wmBulletOOBB);
			switch (containType)
			{
			case DISJOINT:
			{
				int nPlaneIndex = -1;
				for (int k = 0; k < 4; ++k)
				{
					PlaneIntersectionType intersectType = wmBulletOOBB.Intersects(
						XMLoadFloat4(&m_pxmf4WallPlanes[k]));
					if (intersectType == BACK)
					{
						nPlaneIndex = k;
						break;
					}
				}
				if (nPlaneIndex != -1)
				{
					m_pppObjects[ObjectTag::Bullet][j]->SetPosition(0.0f, 100.0f, 0.0f);
					m_pppObjects[ObjectTag::Bullet][j]->Animate(fTimeElapsed);
					m_pppObjects[ObjectTag::Bullet][j]->Sleep();
				}
				break;
			}
			case INTERSECTS:
			{
				int nPlaneIndex = -1;
				for (int k = 0; k < 4; ++k)
				{
					PlaneIntersectionType intersectType = wmBulletOOBB.Intersects(
						XMLoadFloat4(&m_pxmf4WallPlanes[k]));
					if (intersectType == INTERSECTING)
					{
						nPlaneIndex = k;
						break;
					}
				}
				if (nPlaneIndex != -1)
				{
					m_pppObjects[ObjectTag::Bullet][j]->SetPosition(0.0f, 100.0f, 0.0f);
					m_pppObjects[ObjectTag::Bullet][j]->Animate(fTimeElapsed);
					m_pppObjects[ObjectTag::Bullet][j]->Sleep();
				}
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
			case DISJOINT:
			{
				int nPlaneIndex = -1;
				for (int k = 0; k < 4; k++)
				{
					PlaneIntersectionType intersectType = wmParticleOOBB.Intersects(
						XMLoadFloat4(&m_pxmf4WallPlanes[k]));
					if (intersectType == BACK)
					{
						nPlaneIndex = k;
						CRotatingObject* Particle = dynamic_cast<CRotatingObject*>(
							m_pppObjects[ObjectTag::Particle][j]);
						XMVECTOR xmvNormal = XMVectorSet(
							m_pxmf4WallPlanes[nPlaneIndex].x,
							m_pxmf4WallPlanes[nPlaneIndex].y,
							m_pxmf4WallPlanes[nPlaneIndex].z, 0.0f);
						XMVECTOR xmvReflect = XMVector3Reflect(XMLoadFloat3(
							&Particle->GetMovingDirection()), xmvNormal);
						XMStoreFloat3(&Particle->GetMovingDirection(), XMLoadFloat3(
							&Vector3::Normalize(Vector3::XMVectorToFloat3(xmvReflect))));
						Particle->Move(Vector3::Normalize(
							Vector3::Add(XMFLOAT3(
							m_pxmf4WallPlanes[nPlaneIndex].x,
							m_pxmf4WallPlanes[nPlaneIndex].y,
							m_pxmf4WallPlanes[nPlaneIndex].z),
							Vector3::XMVectorToFloat3(xmvReflect))),
							REPULSIVE_FORCE * fTimeElapsed);
					}
				}
				break;
			}
			case INTERSECTS:
			{
				int nPlaneIndex = -1;
				for (int k = 0; k < 4; ++k)
				{
					PlaneIntersectionType intersectType = wmParticleOOBB.Intersects(
						XMLoadFloat4(&m_pxmf4WallPlanes[k]));
					if (intersectType == INTERSECTING)
					{
						nPlaneIndex = k;
						CRotatingObject* Particle = dynamic_cast<CRotatingObject*>(
							m_pppObjects[ObjectTag::Particle][j]);
						XMVECTOR xmvNormal = XMVectorSet(
							m_pxmf4WallPlanes[nPlaneIndex].x,
							m_pxmf4WallPlanes[nPlaneIndex].y,
							m_pxmf4WallPlanes[nPlaneIndex].z, 0.0f);
						XMVECTOR xmvReflect = XMVector3Reflect(XMLoadFloat3(
							&Particle->GetMovingDirection()), xmvNormal);
						XMStoreFloat3(&Particle->GetMovingDirection(), XMLoadFloat3(
							&Vector3::Normalize(Vector3::XMVectorToFloat3(xmvReflect))));
						Particle->Move(Vector3::Normalize(Vector3::Add(XMFLOAT3(
							m_pxmf4WallPlanes[nPlaneIndex].x,
							m_pxmf4WallPlanes[nPlaneIndex].y,
							m_pxmf4WallPlanes[nPlaneIndex].z),
							Vector3::XMVectorToFloat3(xmvReflect))),
							REPULSIVE_FORCE * fTimeElapsed);
					}
				}
				break;
			}
			case CONTAINS:
				break;
			}
		}
		for (int j = 0; j < m_pnObjects[ObjectTag::Objects]; ++j)
		{
			BoundingOrientedBox wmObjectOOBB =
				m_pppObjects[ObjectTag::Objects][j]->GetOOBB();
			ContainmentType containType = wmWallOOBB.Contains(wmObjectOOBB);
			switch (containType)
			{
			case DISJOINT:
			{
				int nPlaneIndex = -1;
				for (int k = 0; k < 4; ++k)
				{
					PlaneIntersectionType intersectType = wmObjectOOBB.Intersects(
						XMLoadFloat4(&m_pxmf4WallPlanes[k]));
					if (intersectType == BACK)
					{
						nPlaneIndex = k;
						CRotatingObject* Object = dynamic_cast<CRotatingObject*>(
							m_pppObjects[ObjectTag::Objects][j]);
						switch (Object->GetObjectTag())
						{
						case CGameObject::ObjectType::RedCube:
						case CGameObject::ObjectType::GreenCube:
							Object->GetMovingDirection() = XMFLOAT3(
								m_pxmf4WallPlanes[nPlaneIndex].x,
								m_pxmf4WallPlanes[nPlaneIndex].y,
								m_pxmf4WallPlanes[nPlaneIndex].z);
							Object->Move(Object->GetMovingDirection(),
								REPULSIVE_FORCE * fTimeElapsed);
							break;
						case CGameObject::ObjectType::BlueCube:
							Object->Move(XMFLOAT3(
								m_pxmf4WallPlanes[nPlaneIndex].x,
								m_pxmf4WallPlanes[nPlaneIndex].y,
								m_pxmf4WallPlanes[nPlaneIndex].z),
								REPULSIVE_FORCE * fTimeElapsed);
							break;
						case CGameObject::ObjectType::WhiteCube:
							XMVECTOR xmvNormal = XMVectorSet(
								m_pxmf4WallPlanes[nPlaneIndex].x,
								m_pxmf4WallPlanes[nPlaneIndex].y,
								m_pxmf4WallPlanes[nPlaneIndex].z, 0.0f);
							XMVECTOR xmvReflect = XMVector3Reflect(XMLoadFloat3(
								&Object->GetMovingDirection()), xmvNormal);
							Object->GetMovingDirection() = Vector3::Normalize(
								Vector3::XMVectorToFloat3(xmvReflect));

							Object->Move(Vector3::Normalize(Vector3::Add(XMFLOAT3A(
								m_pxmf4WallPlanes[nPlaneIndex].x,
								m_pxmf4WallPlanes[nPlaneIndex].y,
								m_pxmf4WallPlanes[nPlaneIndex].z),
								Vector3::XMVectorToFloat3(xmvReflect))),
								REPULSIVE_FORCE * fTimeElapsed);
							break;
						}
					}
				}
				break;
			}
			case INTERSECTS:
			{
				int nPlaneIndex = -1;
				for (int k = 0; k < 4; k++)
				{
					PlaneIntersectionType intersectType = wmObjectOOBB.Intersects(
						XMLoadFloat4(&m_pxmf4WallPlanes[k]));
					if (intersectType == INTERSECTING)
					{
						nPlaneIndex = k;
						CRotatingObject* Object = dynamic_cast<CRotatingObject*>(
							m_pppObjects[ObjectTag::Objects][j]);
						switch (Object->GetObjectTag())
						{
						case CGameObject::ObjectType::RedCube:
						case CGameObject::ObjectType::GreenCube:
							Object->GetMovingDirection() = XMFLOAT3(
								m_pxmf4WallPlanes[nPlaneIndex].x,
								m_pxmf4WallPlanes[nPlaneIndex].y,
								m_pxmf4WallPlanes[nPlaneIndex].z);
							Object->Move(Object->GetMovingDirection(),
								REPULSIVE_FORCE * fTimeElapsed);
							break;
						case CGameObject::ObjectType::BlueCube:
							Object->Move(XMFLOAT3(
								m_pxmf4WallPlanes[nPlaneIndex].x,
								m_pxmf4WallPlanes[nPlaneIndex].y,
								m_pxmf4WallPlanes[nPlaneIndex].z),
								REPULSIVE_FORCE * fTimeElapsed);
							break;
						case CGameObject::ObjectType::WhiteCube:
							XMVECTOR xmvNormal = XMVectorSet(
								m_pxmf4WallPlanes[nPlaneIndex].x,
								m_pxmf4WallPlanes[nPlaneIndex].y,
								m_pxmf4WallPlanes[nPlaneIndex].z, 0.0f);
							XMVECTOR xmvReflect = XMVector3Reflect(XMLoadFloat3(
								&Object->GetMovingDirection()), xmvNormal);
							Object->GetMovingDirection() = Vector3::Normalize(
								Vector3::XMVectorToFloat3(xmvReflect));

							Object->Move(Vector3::Normalize(Vector3::Add(XMFLOAT3A(
								m_pxmf4WallPlanes[nPlaneIndex].x,
								m_pxmf4WallPlanes[nPlaneIndex].y,
								m_pxmf4WallPlanes[nPlaneIndex].z),
								Vector3::XMVectorToFloat3(xmvReflect))),
								REPULSIVE_FORCE * fTimeElapsed);
							break;
						}
					}
				}
				break;
			}
			case CONTAINS:
				break;
			}
		}
		if (m_pPlayer->GetCamera()->GetMode() != THIRD_PERSON_CAMERA)
			continue;
		BoundingOrientedBox wmPlayerOOBB = m_pPlayer->GetOOBB();
		ContainmentType containType = wmWallOOBB.Contains(wmPlayerOOBB);
		switch (containType)
		{
		case DISJOINT:
		{
			for (int j = 0; j < 4; j++)
			{
				PlaneIntersectionType intersectType = wmPlayerOOBB.Intersects(
					XMLoadFloat4(&m_pxmf4WallPlanes[j]));
				if (intersectType == BACK)
				{
					XMFLOAT3 xmvNormal = XMFLOAT3(
						m_pxmf4WallPlanes[j].x,
						m_pxmf4WallPlanes[j].y,
						m_pxmf4WallPlanes[j].z);
					XMFLOAT3 xmvShift = XMFLOAT3(0, 0, 0);
					xmvShift = Vector3::Add(xmvShift, xmvNormal,
						REPULSIVE_FORCE * fTimeElapsed);
					m_pPlayer->Move(xmvShift, false);
				}
			}
			break;
		}
		case INTERSECTS:
		{
			for (int j = 0; j < 4; j++)
			{
				PlaneIntersectionType intersectType = wmPlayerOOBB.Intersects(
					XMLoadFloat4(&m_pxmf4WallPlanes[j]));
				if (intersectType == INTERSECTING)
				{
					XMFLOAT3 xmvNormal = XMFLOAT3(
						m_pxmf4WallPlanes[j].x,
						m_pxmf4WallPlanes[j].y,
						m_pxmf4WallPlanes[j].z);
					XMFLOAT3 xmvShift = XMFLOAT3(0, 0, 0);
					xmvShift = Vector3::Add(xmvShift, xmvNormal,
						REPULSIVE_FORCE * fTimeElapsed);
					m_pPlayer->Move(xmvShift, false);
				}
			}
			break;
		}
		case CONTAINS:
			break;
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
			switch (Object->GetObjectTag())
			{
			case CGameObject::ObjectType::RedCube:
				direct.y = 0.0f;
				direct.z = 0.0f;
				Object->SetMovingDirection(Vector3::Normalize(direct));
				break;
			case CGameObject::ObjectType::GreenCube:
				direct.x = 0.0f;
				direct.z = 0.0f;
				Object->SetMovingDirection(Vector3::Normalize(direct));
				break;
			case CGameObject::ObjectType::BlueCube:
				direct.x = 0.0f;
				direct.y = 0.0f;
				Object->SetMovingDirection(Vector3::Normalize(direct));
				break;
			case CGameObject::ObjectType::WhiteCube:
				Object->SetMovingDirection(direct);
				break;
			}

			direct = Vector3::Normalize(Vector3::Add(
				xmf3ColliderPos, xmf3ObjPos, -1.0f));
			switch (Collider->GetObjectTag())
			{
			case CGameObject::ObjectType::RedCube:
				direct.y = 0.0f;
				direct.z = 0.0f;
				Collider->SetMovingDirection(Vector3::Normalize(direct));
				break;
			case CGameObject::ObjectType::GreenCube:
				direct.x = 0.0f;
				direct.z = 0.0f;
				Collider->SetMovingDirection(Vector3::Normalize(direct));
				break;
			case CGameObject::ObjectType::BlueCube:
				direct.x = 0.0f;
				direct.y = 0.0f;
				Collider->SetMovingDirection(Vector3::Normalize(direct));
				break;
			case CGameObject::ObjectType::WhiteCube:
				Collider->SetMovingDirection(direct);
				break;
			}

			Object->SetMovingSpeed(Collider->GetMovingSpeed());
			Object->Move(Object->GetMovingDirection(),
				REPULSIVE_FORCE * fTimeElapsed);
			Collider->SetMovingSpeed(fMovingSpeed);
			Collider->Move(Collider->GetMovingDirection(),
				REPULSIVE_FORCE * fTimeElapsed);
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
				PopParticles(Object->GetPosition());
				ResetObjects(Object, m_pPlayer->GetPosition().z);
				m_pppObjects[ObjectTag::Bullet][i]->SetPosition(0.0f, 100.0f, 0.0f);
				m_pppObjects[ObjectTag::Bullet][i]->Animate(fTimeElapsed);
				m_pppObjects[ObjectTag::Bullet][i]->Sleep();
			}
		}
	}
}

void CScene::ReleaseUploadBuffers()
{
	for (int i = 0; i < m_nShaders; i++) 
		m_pShaders[i].ReleaseUploadBuffers();
}

bool CScene::OnProcessingMouseMessage(
	HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	switch (nMessageID)
	{
	case WM_LBUTTONDOWN:
	case WM_RBUTTONDOWN:
		//마우스 캡쳐를 하고 현재 마우스 위치를 가져온다.
		::SetCapture(hWnd);
		::GetCursorPos(&m_ptOldCursorPos);
		break;
	case WM_LBUTTONUP:
	case WM_RBUTTONUP:
		//마우스 캡쳐를 해제한다.
		::ReleaseCapture();
		break;
	case WM_MOUSEMOVE:
		break;
	default:
		return false;
	}
	return true;
}
bool CScene::OnProcessingKeyboardMessage(
	HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	return(false);
}

void CScene::ShootBullet()
{
	if (m_fBulletTimer > BulletDelay)
	{
		if (m_pPlayer->GetCamera()->GetMode() != THIRD_PERSON_CAMERA)
			return;
		m_fBulletTimer = 0.0f;
		if (m_nCurrentBulletIdx == m_pnObjects[ObjectTag::Bullet])
			m_nCurrentBulletIdx = 0;
		CRotatingObject *bullet = dynamic_cast<CRotatingObject*>(
			m_pppObjects[ObjectTag::Bullet][m_nCurrentBulletIdx++]);
		bullet->Awake();
		bullet->SetMovingDirection(m_pPlayer->GetLookVector());
		bullet->SetPosition(m_pPlayer->GetPosition());
	}
}

void CScene::ResetObjects(CRotatingObject * Object, float fPlayerPos_Z)
{
	switch (rand() % 4)
	{
	case 0:
		Object->SetObjectType(CGameObject::ObjectType::RedCube);
		Object->SetMovingDirection(XMFLOAT3(1.0f, 0.0f, 0.0f));
		break;
	case 1:
		Object->SetObjectType(CGameObject::ObjectType::GreenCube);
		Object->SetMovingDirection(XMFLOAT3(0.0f, 1.0f, 0.0f));
		break;
	case 2:
		Object->SetObjectType(CGameObject::ObjectType::BlueCube);
		Object->SetMovingDirection(XMFLOAT3(0.0f, 0.0f, -1.0f));
		break;
	case 3:
		Object->SetObjectType(CGameObject::ObjectType::WhiteCube);
		Object->SetMovingDirection(XMFLOAT3(
			(float)RANDOM_NUM(-1000, 1000),
			(float)RANDOM_NUM(-1000, 1000),
			(float)RANDOM_NUM(-1000, 1000)));
		break;
	}
	Object->SetPosition(
		(float)RANDOM_NUM(-15, 15),
		(float)RANDOM_NUM(-15, 15),
		fPlayerPos_Z + (float)RANDOM_NUM(100, 240));
	Object->SetRotationAxis(XMFLOAT3(
		(float)RANDOM_NUM(-1000, 1000),
		(float)RANDOM_NUM(-1000, 1000),
		(float)RANDOM_NUM(-1000, 1000)));
	Object->SetRotationSpeed((float)RANDOM_NUM(50, 100));
	Object->SetMovingSpeed((float)RANDOM_NUM(10, 20));
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

