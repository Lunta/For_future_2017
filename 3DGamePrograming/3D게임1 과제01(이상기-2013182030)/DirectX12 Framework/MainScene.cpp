#include "stdafx.h"
#include "Framework.h"
#include "GameObject.h"
#include "Player.h"
#include "MainScene.h"
#include "MainSceneInputProcessor.h"

using namespace std;

CMainScene::CMainScene() :
	m_pPlayer(nullptr),
	m_pObjectParticleMesh(nullptr),
	m_bGameOver(false),
	m_fRestartCounter(0.0f)
{
}
CMainScene::~CMainScene()
{
	ReleaseObjects();
}

void CMainScene::Animate(float fTimeElapsed)
{
	// Animate all objects
	m_pPlayer->Animate(fTimeElapsed);
	for (CGameObject* Object : m_vecObjects)
		Object->Animate(fTimeElapsed);
	for (CGameObject* Bullet : m_BulletsList)
		Bullet->Animate(fTimeElapsed);
	for (CGameObject* Particle : m_ParticlesList)
		Particle->Animate(fTimeElapsed);

	// Collision check between walls and all objects
	for (CGameObject* wall : m_vecWalls)
	{
		if (m_BulletsList.size() > 0)
		{
			for (list<CGameObject*>::iterator& Bullet = m_BulletsList.begin();
				Bullet != m_BulletsList.end(); )
			{
				ContainmentType containType = wall->m_xmOOBB.Contains((*Bullet)->m_xmOOBB);
				switch (containType)
				{
				case DISJOINT:
				{
					int nPlaneIndex = -1;
					for (int j = 0; j < 4; j++)
					{
						PlaneIntersectionType intersectType = (*Bullet)->m_xmOOBB.Intersects(
							XMLoadFloat4A(&m_pxmf4WallPlanes[j]));
						if (intersectType == BACK)
						{
							nPlaneIndex = j;
							break;
						}
					}
					if (nPlaneIndex != -1)
					{
						delete (*Bullet);
						Bullet = m_BulletsList.erase(Bullet);
					}
					else ++Bullet;
					break;
				}
				case INTERSECTS:
				{
					int nPlaneIndex = -1;
					for (int j = 0; j < 4; j++)
					{
						PlaneIntersectionType intersectType = (*Bullet)->m_xmOOBB.Intersects(
							XMLoadFloat4A(&m_pxmf4WallPlanes[j]));
						if (intersectType == INTERSECTING)
						{
							nPlaneIndex = j;
							break;
						}
					}
					if (nPlaneIndex != -1)
					{
						delete (*Bullet);
						Bullet = m_BulletsList.erase(Bullet);
					}
					else ++Bullet;
					break;
				}
				default:
					++Bullet;
					break;
				}
			}
		}	
		if (m_ParticlesList.size() > 0)
		{
			for (CGameObject* Particle : m_ParticlesList)
			{
				ContainmentType containType = wall->m_xmOOBB.Contains(Particle->m_xmOOBB);
				switch (containType)
				{
				case DISJOINT:
				{
					int nPlaneIndex = -1;
					for (int j = 0; j < 4; j++)
					{
						PlaneIntersectionType intersectType = Particle->m_xmOOBB.Intersects(
							XMLoadFloat4A(&m_pxmf4WallPlanes[j]));
						if (intersectType == BACK)
						{
							nPlaneIndex = j;
							break;
						}
					}
					if (nPlaneIndex != -1)
					{
						XMVECTOR xmvNormal = XMVectorSet(
							m_pxmf4WallPlanes[nPlaneIndex].x,
							m_pxmf4WallPlanes[nPlaneIndex].y,
							m_pxmf4WallPlanes[nPlaneIndex].z, 0.0f);
						XMVECTOR xmvReflect = XMVector3Reflect(XMLoadFloat3A(
							&Particle->m_xmf3MovingDirection), xmvNormal);
						XMStoreFloat3A(&Particle->m_xmf3MovingDirection, XMLoadFloat3A(
							&Vector3::Normalize(Vector3::XMVectorToFloat3A(xmvReflect))));
						Particle->Move(Vector3::Normalize(Vector3::Add(XMFLOAT3A(
							m_pxmf4WallPlanes[nPlaneIndex].x,
							m_pxmf4WallPlanes[nPlaneIndex].y,
							m_pxmf4WallPlanes[nPlaneIndex].z),
							Vector3::XMVectorToFloat3A(xmvReflect))),
							Particle->m_fMovingSpeed * fTimeElapsed);
					}
					break;
				}
				case INTERSECTS:
				{
					int nPlaneIndex = -1;
					for (int j = 0; j < 4; j++)
					{
						PlaneIntersectionType intersectType = Particle->m_xmOOBB.Intersects(
							XMLoadFloat4A(&m_pxmf4WallPlanes[j]));
						if (intersectType == INTERSECTING)
						{
							nPlaneIndex = j;
							break;
						}
					}
					if (nPlaneIndex != -1)
					{
						XMVECTOR xmvNormal = XMVectorSet(
							m_pxmf4WallPlanes[nPlaneIndex].x,
							m_pxmf4WallPlanes[nPlaneIndex].y,
							m_pxmf4WallPlanes[nPlaneIndex].z, 0.0f);
						XMVECTOR xmvReflect = XMVector3Reflect(XMLoadFloat3A(
							&Particle->m_xmf3MovingDirection), xmvNormal);
						XMStoreFloat3A(&Particle->m_xmf3MovingDirection, XMLoadFloat3A(
							&Vector3::Normalize(Vector3::XMVectorToFloat3A(xmvReflect))));
						Particle->Move(Vector3::Normalize(Vector3::Add(XMFLOAT3A(
							m_pxmf4WallPlanes[nPlaneIndex].x,
							m_pxmf4WallPlanes[nPlaneIndex].y,
							m_pxmf4WallPlanes[nPlaneIndex].z),
							Vector3::XMVectorToFloat3A(xmvReflect))),
							Particle->m_fMovingSpeed * fTimeElapsed);
					}
					break;
				}
				case CONTAINS:
					break;
				}
			}
		}
		for (CGameObject* Object : m_vecObjects)
		{
			if (Object->IsStiff()) continue;
			ContainmentType containType = wall->m_xmOOBB.Contains(Object->m_xmOOBB);
			switch (containType)
			{
			case DISJOINT:
			{
				int nPlaneIndex = -1;
				for (int j = 0; j < 4; j++)
				{
					PlaneIntersectionType intersectType = Object->m_xmOOBB.Intersects(
						XMLoadFloat4A(&m_pxmf4WallPlanes[j]));
					if (intersectType == BACK)
					{
						nPlaneIndex = j;
						break;
					}
				}
				if (nPlaneIndex != -1)
				{
					Object->GoStiff();
					switch (Object->m_dwColor)
					{
					case RGB(255, 0, 0):
						Object->m_xmf3MovingDirection.x = -Object->m_xmf3MovingDirection.x;
						Object->Move(Object->m_xmf3MovingDirection,
							Object->m_fMovingSpeed * fTimeElapsed);
						break;
					case RGB(0, 255, 0):
						Object->m_xmf3MovingDirection.y = -Object->m_xmf3MovingDirection.y;
						Object->Move(Object->m_xmf3MovingDirection,
							Object->m_fMovingSpeed * fTimeElapsed);
						break;
					case RGB(0, 0, 255):
						Object->Move(XMFLOAT3A(
							m_pxmf4WallPlanes[nPlaneIndex].x,
							m_pxmf4WallPlanes[nPlaneIndex].y,
							m_pxmf4WallPlanes[nPlaneIndex].z),
							Object->m_fMovingSpeed * fTimeElapsed);
						break;
					case RGB(255, 255, 255):
						XMVECTOR xmvNormal = XMVectorSet(
							m_pxmf4WallPlanes[nPlaneIndex].x,
							m_pxmf4WallPlanes[nPlaneIndex].y,
							m_pxmf4WallPlanes[nPlaneIndex].z, 0.0f);
						XMVECTOR xmvReflect = XMVector3Reflect(XMLoadFloat3A(
							&Object->m_xmf3MovingDirection), xmvNormal);
						Object->m_xmf3MovingDirection = Vector3::Normalize(
							Vector3::XMVectorToFloat3A(xmvReflect));

						Object->Move(Vector3::Normalize(Vector3::Add(XMFLOAT3A(
							m_pxmf4WallPlanes[nPlaneIndex].x,
							m_pxmf4WallPlanes[nPlaneIndex].y,
							m_pxmf4WallPlanes[nPlaneIndex].z),
							Vector3::XMVectorToFloat3A(xmvReflect))),
							Object->m_fMovingSpeed * fTimeElapsed);
						break;
					}
				}
				break;
			}
			case INTERSECTS:
			{
				int nPlaneIndex = -1;
				for (int j = 0; j < 4; j++)
				{
					PlaneIntersectionType intersectType = Object->m_xmOOBB.Intersects(
						XMLoadFloat4A(&m_pxmf4WallPlanes[j]));
					if (intersectType == INTERSECTING)
					{
						nPlaneIndex = j;
						break;
					}
				}
				if (nPlaneIndex != -1)
				{
					Object->GoStiff();
					switch (Object->m_dwColor)
					{
					case RGB(255, 0, 0):
						Object->m_xmf3MovingDirection.x = -Object->m_xmf3MovingDirection.x;
						Object->Move(Object->m_xmf3MovingDirection,
							Object->m_fMovingSpeed * fTimeElapsed);
						break;
					case RGB(0, 255, 0):
						Object->m_xmf3MovingDirection.y = -Object->m_xmf3MovingDirection.y;
						Object->Move(Object->m_xmf3MovingDirection,
							Object->m_fMovingSpeed * fTimeElapsed);
						break;
					case RGB(0, 0, 255):
						Object->Move(XMFLOAT3A(
							m_pxmf4WallPlanes[nPlaneIndex].x,
							m_pxmf4WallPlanes[nPlaneIndex].y,
							m_pxmf4WallPlanes[nPlaneIndex].z),
							Object->m_fMovingSpeed * fTimeElapsed);
						break;
					case RGB(255, 255, 255):
						XMVECTOR xmvNormal = XMVectorSet(
							m_pxmf4WallPlanes[nPlaneIndex].x,
							m_pxmf4WallPlanes[nPlaneIndex].y,
							m_pxmf4WallPlanes[nPlaneIndex].z, 0.0f);
						XMVECTOR xmvReflect = XMVector3Reflect(XMLoadFloat3A(
							&Object->m_xmf3MovingDirection), xmvNormal);

						Object->m_xmf3MovingDirection = Vector3::Normalize(
							Vector3::XMVectorToFloat3A(xmvReflect));

						Object->Move(Vector3::Normalize(Vector3::Add(XMFLOAT3A(
							m_pxmf4WallPlanes[nPlaneIndex].x,
							m_pxmf4WallPlanes[nPlaneIndex].y,
							m_pxmf4WallPlanes[nPlaneIndex].z),
							Vector3::XMVectorToFloat3A(xmvReflect))),
							Object->m_fMovingSpeed * fTimeElapsed);
						break;
					}
				}
				break;
			}
			case CONTAINS:
				break;
			}
		}
		ContainmentType containType = wall->m_xmOOBB.Contains(m_pPlayer->m_xmOOBB);
		switch (containType)
		{
		case DISJOINT:
		{
			for (int j = 0; j < 4; j++)
			{
				PlaneIntersectionType intersectType = m_pPlayer->m_xmOOBB.Intersects(
					XMLoadFloat4A(&m_pxmf4WallPlanes[j]));
				if (intersectType == BACK)
				{
					XMFLOAT3A xmvNormal = XMFLOAT3A(
						m_pxmf4WallPlanes[j].x,
						m_pxmf4WallPlanes[j].y,
						m_pxmf4WallPlanes[j].z);
					XMFLOAT3A xmvShift = XMFLOAT3A(0, 0, 0);
					xmvShift = Vector3::Add(xmvShift, xmvNormal,
						m_pPlayer->m_fSpeed * fTimeElapsed);
					m_pPlayer->Move(xmvShift, false);
				}
			}
			break;
		}
		case INTERSECTS:
		{
			for (int j = 0; j < 4; j++)
			{
				PlaneIntersectionType intersectType = m_pPlayer->m_xmOOBB.Intersects(
					XMLoadFloat4A(&m_pxmf4WallPlanes[j]));
				if (intersectType == INTERSECTING)
				{
					XMFLOAT3A xmvNormal = XMFLOAT3A(
						m_pxmf4WallPlanes[j].x,
						m_pxmf4WallPlanes[j].y,
						m_pxmf4WallPlanes[j].z);
					XMFLOAT3A xmvShift = XMFLOAT3A(0, 0, 0);
					xmvShift = Vector3::Add(xmvShift, xmvNormal,
						m_pPlayer->m_fSpeed * fTimeElapsed);
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
	for (CGameObject* Object : m_vecObjects)
		Object->m_pCollider = NULL;
	for (vector<CGameObject*>::iterator &Object1 = m_vecObjects.begin(); 
		Object1 != m_vecObjects.end(); ++Object1)
	{
		if ((*Object1)->IsStiff()) continue;
		for (vector<CGameObject*>::iterator Object2 = Object1;
			Object2 != --m_vecObjects.end();)
		{
			++Object2;
			if ((*Object2)->IsStiff()) continue;
			if ((*Object1)->m_xmOOBB.Intersects((*Object2)->m_xmOOBB))
			{
				(*Object1)->m_pCollider = *Object2;
				(*Object2)->m_pCollider = *Object1;
			}
		}
		if (!m_bGameOver && m_pPlayer->m_xmOOBB.Intersects((*Object1)->m_xmOOBB))
		{
			BuildParticles(m_pPlayer->GetPosition(), 300);
			m_bGameOver = true;
		}
	}
	for (CGameObject* Object : m_vecObjects)
	{
		if (Object->IsStiff()) continue;
		if (Object->m_pCollider && Object->m_pCollider->m_pCollider)
		{
			CGameObject* Collider = Object->m_pCollider;
			float fMovingSpeed = Object->m_fMovingSpeed;
			XMFLOAT3A xmf3ObjPos = Object->GetPosition();
			XMFLOAT3A xmf3ColliderPos = Collider->GetPosition();

			switch (Object->m_dwColor)
			{
			case RGB(255, 0, 0):
				Object->m_xmf3MovingDirection.x = -Object->m_xmf3MovingDirection.x;
				break;
			case RGB(0, 255, 0):
				Object->m_xmf3MovingDirection.y = -Object->m_xmf3MovingDirection.y;
				break;
			case RGB(0, 0, 255):
				Object->m_xmf3MovingDirection.z = -Object->m_xmf3MovingDirection.z;
				break;
			case RGB(255, 255, 255):
				Object->m_xmf3MovingDirection = Vector3::Normalize(Vector3::Add(
					xmf3ObjPos, xmf3ColliderPos, -1.0f));
				break;
			}

			switch (Collider->m_dwColor)
			{
			case RGB(255, 0, 0):
				Collider->m_xmf3MovingDirection.x = -Collider->m_xmf3MovingDirection.x;
				break;
			case RGB(0, 255, 0):
				Collider->m_xmf3MovingDirection.y = -Collider->m_xmf3MovingDirection.y;
				break;
			case RGB(0, 0, 255):
				Collider->m_xmf3MovingDirection.z = -Collider->m_xmf3MovingDirection.z;
				break;
			case RGB(255, 255, 255):
				 Collider->m_xmf3MovingDirection = Vector3::Normalize(Vector3::Add(
					 xmf3ColliderPos, xmf3ObjPos, -1.0f));
				break;
			}

			Object->m_fMovingSpeed = Collider->m_fMovingSpeed;
			Object->Move(Object->m_xmf3MovingDirection,
				Object->m_fMovingSpeed * fTimeElapsed);
			Collider->m_fMovingSpeed = fMovingSpeed;
			Collider->Move(Collider->m_xmf3MovingDirection,
				Collider->m_fMovingSpeed * fTimeElapsed);
			Object->m_pCollider = NULL;
			Collider->m_pCollider = NULL;
			Object->GoStiff();
			Collider->GoStiff();
		}
	}
	if (m_BulletsList.size() > 0) 
	{
		for (list<CGameObject*>::iterator& Bullet = m_BulletsList.begin();
			Bullet != m_BulletsList.end(); )
		{
			for (CGameObject* Object : m_vecObjects)
			{
				if ((*Bullet)->m_xmOOBB.Intersects(Object->m_xmOOBB))
				{
					(*Bullet)->GoStiff();
					BuildParticles(Object->GetPosition());
					switch (rand() % 4)
					{
					case 0:
						Object->SetColor(RGB(255, 0, 0));
						Object->SetMovingDirection(XMFLOAT3A(1.0f, 0.0f, 0.0f));
						break;
					case 1:
						Object->SetColor(RGB(0, 255, 0));
						Object->SetMovingDirection(XMFLOAT3A(0.0f, 1.0f, 0.0f));
						break;
					case 2:
						Object->SetColor(RGB(0, 0, 255));
						Object->SetMovingDirection(XMFLOAT3A(0.0f, 0.0f, -1.0f));
						break;
					case 3:
						Object->SetColor(RGB(255, 255, 255));
						Object->SetMovingDirection(XMFLOAT3A(
							(float)RANDOM_NUM(-1000, 1000),
							(float)RANDOM_NUM(-1000, 1000),
							(float)RANDOM_NUM(-1000, 1000)));
						break;
					}
					float player_z = m_pPlayer->GetPosition().z;
					Object->SetPosition(
						(float)RANDOM_NUM(-15, 15),
						(float)RANDOM_NUM(-15, 15),
						player_z + (float)RANDOM_NUM(100, 240));
					Object->SetRotationAxis(XMFLOAT3A(
						(float)RANDOM_NUM(-1000, 1000),
						(float)RANDOM_NUM(-1000, 1000),
						(float)RANDOM_NUM(-1000, 1000)));
					Object->SetRotationSpeed((float)RANDOM_NUM(50, 100));
					Object->SetMovingSpeed((float)RANDOM_NUM(10, 20));
					Object->SetOOBB(
						XMFLOAT3A(0.0f, 0.0f, 0.0f),
						XMFLOAT3A(2.0f, 2.0f, 2.0f),
						XMFLOAT4A(0.0f, 0.0f, 0.0f, 1.0f));

				}
			}
			if ((*Bullet)->IsStiff())
			{
				delete (*Bullet);
				Bullet = m_BulletsList.erase(Bullet);
			}
			else ++Bullet;
		}
	}
}

void CMainScene::Update(float fTimeElapsed)
{
	XMFLOAT3A PlayerPos = m_pPlayer->GetPosition();
	if (m_bGameOver)
	{
		m_fRestartCounter += fTimeElapsed;
		if (m_fRestartCounter > 2)
		{
			m_fRestartCounter = 0.0f;
			m_bGameOver = false;
			m_pPlayer->Reset();

			for (CGameObject* Object : m_vecObjects)
			{
				if (Object->GetPosition().z > PlayerPos.z - WALL_SIZE &&
					Object->GetPosition().z < PlayerPos.z + WALL_SIZE)
				{
					switch (rand() % 4)
					{
					case 0:
						Object->SetColor(RGB(255, 0, 0));
						Object->SetMovingDirection(XMFLOAT3A(1.0f, 0.0f, 0.0f));
						break;
					case 1:
						Object->SetColor(RGB(0, 255, 0));
						Object->SetMovingDirection(XMFLOAT3A(0.0f, 1.0f, 0.0f));
						break;
					case 2:
						Object->SetColor(RGB(0, 0, 255));
						Object->SetMovingDirection(XMFLOAT3A(0.0f, 0.0f, -1.0f));
						break;
					case 3:
						Object->SetColor(RGB(255, 255, 255));
						Object->SetMovingDirection(XMFLOAT3A(
							(float)RANDOM_NUM(-1000, 1000),
							(float)RANDOM_NUM(-1000, 1000),
							(float)RANDOM_NUM(-1000, 1000)));
						break;
					}
					float Playe_z = m_pPlayer->GetPosition().z;
					Object->SetPosition(
						(float)RANDOM_NUM(-15, 15),
						(float)RANDOM_NUM(-15, 15),
						Playe_z + (float)RANDOM_NUM(100, 240));
					Object->SetRotationAxis(XMFLOAT3A(
						(float)RANDOM_NUM(-1000, 1000),
						(float)RANDOM_NUM(-1000, 1000),
						(float)RANDOM_NUM(-1000, 1000)));
					Object->SetRotationSpeed((float)RANDOM_NUM(50, 100));
					Object->SetMovingSpeed((float)RANDOM_NUM(10, 20));
					Object->SetOOBB(
						XMFLOAT3A(0.0f, 0.0f, 0.0f),
						XMFLOAT3A(2.0f, 2.0f, 2.0f),
						XMFLOAT4A(0.0f, 0.0f, 0.0f, 1.0f));
				}
			}
		}
	}
	else
	{
		m_pInputProcessor->ProcessInput(fTimeElapsed);
	}

	Animate(fTimeElapsed);
	m_pPlayer->Update(fTimeElapsed);

	for (CGameObject* Wall : m_vecWalls)
	{
		if (Wall->GetPosition().z < PlayerPos.z - 240.0f)
		{
			Wall->MoveForward(490.0f);
			Wall->SetOOBB(
				Wall->GetPosition(),
				XMFLOAT3A(WALL_HALFSIZE, WALL_HALFSIZE, 5.0f),
				XMFLOAT4A(0.0f, 0.0f, 0.0f, 1.0f));
		}
	}

	for (list<CGameObject*>::iterator& Bullet = m_BulletsList.begin();
		Bullet != m_BulletsList.end(); )
	{
		XMFLOAT3A BulletPos = (*Bullet)->GetPosition();
		if (BulletPos.x > WALL_HALFSIZE ||
			BulletPos.x < -WALL_HALFSIZE ||
			BulletPos.y > WALL_HALFSIZE ||
			BulletPos.y < -WALL_HALFSIZE ||
			BulletPos.z < PlayerPos.z - 240.0f ||
			BulletPos.z > PlayerPos.z + 240.0f)
		{
			delete (*Bullet);
			Bullet = m_BulletsList.erase(Bullet);
		}
		else ++Bullet;
	}

	for (CGameObject* Object : m_vecObjects)
	{
		XMFLOAT3A ObjectPos = Object->GetPosition();
		if (ObjectPos.x > WALL_HALFSIZE ||
			ObjectPos.x < -WALL_HALFSIZE ||
			ObjectPos.y > WALL_HALFSIZE ||
			ObjectPos.y < -WALL_HALFSIZE ||
			ObjectPos.z < PlayerPos.z - 240.0f ||
			ObjectPos.z > PlayerPos.z + 240.0f)
		{
			switch (rand() % 4)
			{
			case 0:
				Object->SetColor(RGB(255, 0, 0));
				Object->SetMovingDirection(XMFLOAT3A(1.0f, 0.0f, 0.0f));
				break;
			case 1:
				Object->SetColor(RGB(0, 255, 0));
				Object->SetMovingDirection(XMFLOAT3A(0.0f, 1.0f, 0.0f));
				break;
			case 2:
				Object->SetColor(RGB(0, 0, 255));
				Object->SetMovingDirection(XMFLOAT3A(0.0f, 0.0f, -1.0f));
				break;
			case 3:
				Object->SetColor(RGB(255, 255, 255));
				Object->SetMovingDirection(XMFLOAT3A(
					(float)RANDOM_NUM(-1000, 1000),
					(float)RANDOM_NUM(-1000, 1000),
					(float)RANDOM_NUM(-1000, 1000)));
				break;
			}
			Object->SetPosition(
				(float)RANDOM_NUM(-15, 15),
				(float)RANDOM_NUM(-15, 15),
				PlayerPos.z + (float)RANDOM_NUM(100, 240));
			Object->SetRotationAxis(XMFLOAT3A(
				(float)RANDOM_NUM(-1000, 1000),
				(float)RANDOM_NUM(-1000, 1000),
				(float)RANDOM_NUM(-1000, 1000)));
			Object->SetRotationSpeed((float)RANDOM_NUM(50, 100));
			Object->SetMovingSpeed((float)RANDOM_NUM(10, 20));
			Object->SetOOBB(
				XMFLOAT3A(0.0f, 0.0f, 0.0f),
				XMFLOAT3A(2.0f, 2.0f, 2.0f),
				XMFLOAT4A(0.0f, 0.0f, 0.0f, 1.0f));
		}
	}

	for (list<CGameObject*>::iterator& Particle = m_ParticlesList.begin();
		Particle != m_ParticlesList.end(); )
	{
		(*Particle)->m_fMovingRange -= fTimeElapsed;
		if ((*Particle)->m_fMovingRange < 0)
		{
			delete (*Particle);
			Particle = m_ParticlesList.erase(Particle);
		}
		else ++Particle;
	}
}

void CMainScene::Draw(HDC hdc)
{
	for (CGameObject* wall: m_vecWalls)
		wall->Render(hdc, m_pPlayer->m_pCamera);
	
	for (CGameObject* Object : m_vecObjects)
		Object->Render(hdc, m_pPlayer->m_pCamera);
	
	if(!m_bGameOver)
		m_pPlayer->Render(hdc, m_pPlayer->m_pCamera);

	for (CGameObject* Bullet : m_BulletsList)
		Bullet->Render(hdc, m_pPlayer->m_pCamera);

	for (CGameObject* Particle : m_ParticlesList)
		Particle->Render(hdc, m_pPlayer->m_pCamera);
}

bool CMainScene::OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	switch (nMessageID)
	{
	case WM_KEYDOWN:
		break;
	case WM_KEYUP:
		break;
	}
	return false;
}
bool CMainScene::OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	switch (nMessageID)
	{
	case WM_LBUTTONDOWN:
	case WM_RBUTTONDOWN:
		m_pInputProcessor->SetMouseCapture(hWnd);
		break;
	case WM_LBUTTONUP:
	case WM_RBUTTONUP:
		m_pInputProcessor->ReleaseMouseCapture();
		break;
	case WM_MOUSEMOVE:
		break;
	default:
		return false;
	}
	return true;
}
bool CMainScene::OnProcessingWindowMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	switch (nMessageID)
	{
	case WM_SIZE:
		break;
	case WM_DESTROY:
		break;
	}
	return false;
}

void CMainScene::BuildObjects(CFramework * myFramework, CSoundManager *pSoundManager, HWND hWnd, CurrentScene tag)
{
	CScene::BuildObjects(myFramework, pSoundManager, hWnd, tag);

	CAirplaneMesh *pAirplaneMesh = new CAirplaneMesh(6.0f, 6.0f, 1.0f);
	m_pPlayer = new CPlayer();
	m_pPlayer->SetCameraViewport(m_rcScene.left, m_rcScene.top, m_rcScene.right - m_rcScene.left, m_rcScene.bottom - m_rcScene.top);
	m_pPlayer->SetBulletList(&m_BulletsList);
	m_pPlayer->SetPosition(0.0f, 0.0f, 0.0f);
	m_pPlayer->SetMesh(pAirplaneMesh);
	m_pPlayer->SetColor(RGB(0, 0, 255));
	m_pPlayer->SetCameraOffset(XMFLOAT3A(0.0f, 5.0f, -15.0f));
	m_pPlayer->SetShootDelay(0.1f);
	m_pPlayer->SetOOBB(
		XMFLOAT3A(0.0f, 0.0f, 0.0f), 
		XMFLOAT3A(3.0f, 3.0f, 0.5f), 
		XMFLOAT4A(0.0f, 0.0f, 0.0f, 1.0f));

	CWallMesh *pWallCubeMesh = new CWallMesh(WALL_SIZE, WALL_SIZE, 10.0f);
	for (int i = -24; i < 25; i++)
	{
		CGameObject* Wall = new CGameObject();
		Wall->SetPosition(0.0f, 0.0f, 10.0f * i);
		Wall->SetMesh(pWallCubeMesh);
		Wall->SetColor(RGB(127, 127, 127));
		Wall->SetOOBB(
			XMFLOAT3A(0.0f, 0.0f, 10.0f * i),
			XMFLOAT3A(WALL_HALFSIZE, WALL_HALFSIZE, 5.0f),
			XMFLOAT4A(0.0f, 0.0f, 0.0f, 1.0f));
		m_vecWalls.push_back(Wall);
	}
	m_pxmf4WallPlanes[0] = XMFLOAT4A(+1.0f, 0.0f, 0.0f, 20.0f);
	m_pxmf4WallPlanes[1] = XMFLOAT4A(-1.0f, 0.0f, 0.0f, 20.0f);
	m_pxmf4WallPlanes[2] = XMFLOAT4A(0.0f, +1.0f, 0.0f, 20.0f);
	m_pxmf4WallPlanes[3] = XMFLOAT4A(0.0f, -1.0f, 0.0f, 20.0f);
	m_pxmf4WallPlanes[4] = XMFLOAT4A(0.0f, 0.0f, +1.0f, 5.0f);
	m_pxmf4WallPlanes[5] = XMFLOAT4A(0.0f, 0.0f, -1.0f, 5.0f);

	CCubeMesh *pObjectCubeMesh = new CCubeMesh(4.0f, 4.0f, 4.0f);
	m_nObjects = 20;
	while(m_vecObjects.size() < m_nObjects)
	{
		CGameObject* Cube = new CGameObject();
		Cube->SetMesh(pObjectCubeMesh);
		switch (rand()%4)
		{
		case 0: 
			Cube->SetColor(RGB(255, 0, 0));
			Cube->SetMovingDirection(XMFLOAT3A(1.0f, 0.0f, 0.0f));
			break;
		case 1: 
			Cube->SetColor(RGB(0, 255, 0));
			Cube->SetMovingDirection(XMFLOAT3A(0.0f, 1.0f, 0.0f));
			break;
		case 2: 
			Cube->SetColor(RGB(0, 0, 255));
			Cube->SetMovingDirection(XMFLOAT3A(0.0f, 0.0f, -1.0f));
			break;
		case 3: 
			Cube->SetColor(RGB(255, 255, 255));
			Cube->SetMovingDirection(XMFLOAT3A(
				(float)RANDOM_NUM(-1000, 1000),
				(float)RANDOM_NUM(-1000, 1000),
				(float)RANDOM_NUM(-1000, 1000)));
			break;
		}
		float player_z = m_pPlayer->GetPosition().z;
		Cube->SetPosition(
			(float)RANDOM_NUM(-15, 15),
			(float)RANDOM_NUM(-15, 15),
			player_z + (float)RANDOM_NUM(100, 240));
		Cube->SetRotationAxis(XMFLOAT3A(
			(float)RANDOM_NUM(-1000, 1000),
			(float)RANDOM_NUM(-1000, 1000),
			(float)RANDOM_NUM(-1000, 1000)));
		Cube->SetRotationSpeed((float)RANDOM_NUM(50, 100));
		Cube->SetMovingSpeed((float)RANDOM_NUM(10, 20));
		Cube->SetStiffTime(0.0f);
		Cube->SetOOBB(
			XMFLOAT3A(0.0f, 0.0f, 0.0f),
			XMFLOAT3A(2.0f, 2.0f, 2.0f),
			XMFLOAT4A(0.0f, 0.0f, 0.0f, 1.0f));
		m_vecObjects.push_back(Cube);
	}
	m_pInputProcessor = new CMainSceneInputProcessor(
		m_hWnd, CInputProcessor::tagBeingIn::MainScene, m_pPlayer);
}

void CMainScene::ReleaseObjects()
{
	for (CGameObject* wall : m_vecWalls)
		delete wall;
	m_vecWalls.clear();
	for (CGameObject* object : m_vecObjects)
		delete object;
	m_vecObjects.clear();
	for (CGameObject* bullet : m_BulletsList)
		delete bullet;
	m_BulletsList.clear();
	for (CGameObject* particle : m_ParticlesList)
		delete particle;
	m_ParticlesList.clear();
	delete m_pPlayer;
	delete m_pObjectParticleMesh;
}

void CMainScene::BuildParticles(XMFLOAT3A & xmf3Position, int nParticles)
{
	if(m_ParticlesList.size() == 0)
		m_pObjectParticleMesh = new CCubeMesh(1.0f, 1.0f, 1.0f);

	for (int i = 0; i < nParticles; i++)
	{
		CGameObject* Particle = new CGameObject();
		Particle->SetMesh(m_pObjectParticleMesh);

		Particle->SetColor(RGB(
			RANDOM_NUM(0, 255), 
			RANDOM_NUM(0, 255), 
			RANDOM_NUM(0, 255)));
		Particle->SetMovingDirection(Vector3::Normalize(XMFLOAT3A(
			(float)RANDOM_NUM(-1000, 1000),
			(float)RANDOM_NUM(-1000, 1000),
			(float)RANDOM_NUM(-1000, 1000))));
		Particle->SetMovingRange(0.3f);
		Particle->SetPosition(xmf3Position);
		Particle->SetRotationAxis(XMFLOAT3A(
			(float)RANDOM_NUM(-1000, 1000),
			(float)RANDOM_NUM(-1000, 1000),
			(float)RANDOM_NUM(-1000, 1000)));
		Particle->SetRotationSpeed((float)RANDOM_NUM(50, 100));
		Particle->SetMovingSpeed(40.0f);
		Particle->SetOOBB(
			XMFLOAT3A(0.0f, 0.0f, 0.0f),
			XMFLOAT3A(0.5f, 0.5f, 0.5f),
			XMFLOAT4A(0.0f, 0.0f, 0.0f, 1.0f));
		m_ParticlesList.push_back(Particle);
	}
}
