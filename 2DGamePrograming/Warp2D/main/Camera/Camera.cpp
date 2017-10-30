#include "stdafx.h"
#include "Camera.h"

CCamera::CCamera()
{
	m_d2dmtxWorld = Matrix3x2F::Identity();
	m_d2dptPosition = Point2F(0.f, 0.f);
	m_bControlLock = false;
	m_fScale = 1.f;
}

CCamera::~CCamera()
{
}

void CCamera::SetClientSize(D2D_POINT_2F ptClientSize)
{
	m_d2dptClientSize = ptClientSize;
}

void CCamera::SetMatrix()
{
	auto pos = GetCameraPosition();

	m_d2dmtxWorld = Matrix3x2F::Identity();
	m_d2dmtxWorld = Matrix3x2F::Scale(m_fScale, m_fScale) * m_d2dmtxWorld;
	m_d2dmtxWorld._31 = pos.x;
	m_d2dmtxWorld._32 = pos.y;
}

D2D1_POINT_2F CCamera::GetCameraPosition()
{
	return D2D1_POINT_2F
	{
	  Interpolation(m_d2dptPosition.x - m_d2dptClientSize.x * 2.f, m_d2dptPosition.x, m_d2dptAnchor.x * -0.5f + 0.5f)
	, Interpolation(m_d2dptPosition.y - m_d2dptClientSize.y * 2.f, m_d2dptPosition.y, m_d2dptAnchor.y * -0.5f + 0.5f)
	};
}

D2D_MATRIX_3X2_F CCamera::RegenerateViewMatrix()
{
	auto pos = GetCameraPosition();

	SetMatrix();
	D2D_POINT_2F m_vRight = { m_d2dmtxWorld._11, m_d2dmtxWorld._12 };
	D2D_POINT_2F m_vUp = { m_d2dmtxWorld._21, m_d2dmtxWorld._22 };
	m_d2dmtxView._11 = m_vRight.x;
	m_d2dmtxView._21 = m_vRight.y;
	m_d2dmtxView._12 = m_vUp.x;
	m_d2dmtxView._22 = m_vUp.y;
	m_d2dmtxView._31 = -(pos * m_vRight);
	m_d2dmtxView._32 = -(pos * m_vUp);
	return m_d2dmtxView;
}
