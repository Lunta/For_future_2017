#include "stdafx.h"
#include "UIBase.h"

CUIBase::CUIBase()
{
}

CUIBase::~CUIBase()
{
}

void CUIBase::Draw(ID2D1HwndRenderTarget * pd2dRenderTarget)
{
	if (!m_bViewUI) return;
	// Caption
	pd2dRenderTarget->SetTransform(Matrix3x2F::Translation(m_ptOrigin.x, m_ptOrigin.y));

	pd2dRenderTarget->PushAxisAlignedClip(
		  m_rcCaption
		, D2D1_ANTIALIAS_MODE_PER_PRIMITIVE);

	DrawCaption(pd2dRenderTarget);

	pd2dRenderTarget->PopAxisAlignedClip();

	// Client
	pd2dRenderTarget->SetTransform(Matrix3x2F::Translation(m_ptOrigin.x, m_ptOrigin.y + m_rcCaption.bottom));

	pd2dRenderTarget->PushAxisAlignedClip(
		  m_rcClient
		, D2D1_ANTIALIAS_MODE_PER_PRIMITIVE);

	DrawClient(pd2dRenderTarget);

	pd2dRenderTarget->PopAxisAlignedClip();
}

void CUIBase::DrawClient(ID2D1HwndRenderTarget * pd2dRenderTarget)
{
}

void CUIBase::DrawCaption(ID2D1HwndRenderTarget * pd2dRenderTarget)
{

}

void CUIBase::BuildObject(CScene * scene)
{
}
