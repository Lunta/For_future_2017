#include "stdafx.h"
#include "Object.h"

CObject::CObject(D2D_POINT_2F pt, D2D_RECT_F rc)
	: m_ptPoisition{ pt }
	, m_rcSize{ rc }
{ 
}

CObject::~CObject()
{
}
