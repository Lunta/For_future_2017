#include "stdafx.h"
#include "GameObject.h"

#include "Shader.h"

CGameObject::CGameObject()
{
	XMStoreFloat4x4(&m_xmf4x4World, XMMatrixIdentity());
	m_pMesh = NULL;
	m_pShader = NULL;
	m_pCollider = NULL;
	m_bActive = true;
	m_bUpdatedWorldMtx = false;
}
CGameObject::~CGameObject()
{
	if (m_pMesh) m_pMesh->Release();
	if (m_pShader)
	{
		m_pShader->ReleaseShaderVariables();
		m_pShader->Release();
	}
}

void CGameObject::SetMesh(CMesh *pMesh)
{
	if (m_pMesh) m_pMesh->Release();
	m_pMesh = pMesh;
	if (m_pMesh) m_pMesh->AddRef();
}
void CGameObject::SetShader(CShader *pShader)
{
	if (m_pShader) m_pShader->Release();
	m_pShader = pShader;
	if (m_pShader) m_pShader->AddRef();
}

void CGameObject::CreateShaderVariables(ID3D12Device *pd3dDevice,
	ID3D12GraphicsCommandList *pd3dCommandList)
{
}
void CGameObject::ReleaseShaderVariables()
{
}
void CGameObject::UpdateShaderVariables(ID3D12GraphicsCommandList *pd3dCommandList)
{
	XMFLOAT4X4 xmf4x4World;
	XMStoreFloat4x4(&xmf4x4World, XMMatrixTranspose(XMLoadFloat4x4(&m_xmf4x4World)));
	//객체의 월드 변환 행렬을 루트 상수(32-비트 값)를 통하여 셰이더 변수(상수 버퍼)로 복사한다.
	pd3dCommandList->SetGraphicsRoot32BitConstants(0, 16, &xmf4x4World, 0);
}

void CGameObject::OnPrepareRender()
{

}
void CGameObject::Render(ID3D12GraphicsCommandList *pd3dCommandList, 
	CCamera *pCamera)
{
	if (!m_bActive) return;
	if (IsVisible(pCamera))
	{
		//객체의 정보를 셰이더 변수(상수 버퍼)로 복사한다.
		UpdateShaderVariables(pd3dCommandList);
		if (m_pShader) m_pShader->Render(pd3dCommandList, pCamera);
		if (m_pMesh) m_pMesh->Render(pd3dCommandList);
	}
}
void CGameObject::Render(ID3D12GraphicsCommandList * pd3dCommandList, 
	CCamera * pCamera, UINT nInstances)
{
	//if (!m_bActive) return;
	OnPrepareRender();
	if (m_pMesh) m_pMesh->Render(pd3dCommandList, nInstances);
	
}
//인스턴싱 정점 버퍼 뷰를 사용하여 메쉬를 렌더링한다.
void CGameObject::Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera,
	UINT nInstances, D3D12_VERTEX_BUFFER_VIEW d3dInstancingBufferView)
{
	if (!m_bActive) return;
	OnPrepareRender();
	if (m_pMesh)
		m_pMesh->Render(pd3dCommandList, nInstances, d3dInstancingBufferView);
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

bool CGameObject::IsVisible(CCamera * pCamera)
{
	OnPrepareRender();
	
	bool bIsVisible = false;

	//BoundingOrientedBox xmBoundingBox = m_pMesh->GetBoundingBox();
	//
	////모델 좌표계의 바운딩 박스를 월드 좌표계로 변환한다.
	//xmBoundingBox.Transform(xmBoundingBox, XMLoadFloat4x4(&m_xmf4x4World));

	if (pCamera) bIsVisible = pCamera->IsInFrustum(m_xmOOBB);
	return(bIsVisible);
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
	XMMATRIX mtxRotate = XMMatrixRotationRollPitchYaw(XMConvertToRadians(fPitch),
		XMConvertToRadians(fYaw), XMConvertToRadians(fRoll));
	m_xmf4x4World = Matrix4x4::Multiply(mtxRotate, m_xmf4x4World);
	m_bUpdatedWorldMtx = true;
}

void CGameObject::ReleaseUploadBuffers()
{
	//정점 버퍼를 위한 업로드 버퍼를 소멸시킨다.
	if (m_pMesh) m_pMesh->ReleaseUploadBuffers();
}


CRotatingObject::CRotatingObject()
{
	m_xmf3RotationAxis = XMFLOAT3(0.0f, 1.0f, 0.0f);
	m_fRotationSpeed = 90.0f;

	m_xmf3MovingDirection = XMFLOAT3A(0.0f, 0.0f, 1.0f);
	m_fMovingSpeed = 0.0f;
	m_fMovingRange = 0.0f;
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
	XMStoreFloat4(&m_xmOOBBTransformed.Orientation, XMQuaternionNormalize(XMLoadFloat4(&m_xmOOBBTransformed.Orientation)));
	m_bUpdatedWorldMtx = false;
}