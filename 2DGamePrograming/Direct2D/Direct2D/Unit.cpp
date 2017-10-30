#include "stdafx.h"
#include "Unit.h"

CUnit::CUnit()
{
	m_UnitPos = { RANDOM_NUM(0, 600), RANDOM_NUM(0, 600) };
	m_UnitLength = { 30, 30 };

	for (int i = 0; i < RANDOM_NUM(5, 30); i++)
		m_name[i] = RANDOM_NUM(97, 122);

	m_Level = RANDOM_NUM(1, 5);
	m_hp = RANDOM_NUM(30, 100);
	m_mp = RANDOM_NUM(10, 50);
	m_Atk = RANDOM_NUM(1, 5);
	m_Def = RANDOM_NUM(1, 3);
	m_Speed = RANDOM_NUM(10, 50);
}
CUnit::CUnit(RECT rc)
{
	m_UnitPos = { RANDOM_NUM(rc.left, rc.right - 30), RANDOM_NUM(rc.top, rc.bottom - 30) };
	m_UnitLength = { 30, 30 };

	for(int i = 0; i < RANDOM_NUM(5, 30); i++)
		m_name[i] = RANDOM_NUM(97, 122);

	m_Level = RANDOM_NUM(1, 5);
	m_hp = RANDOM_NUM(30, 100);
	m_mp = RANDOM_NUM(10, 50);
	m_Atk = RANDOM_NUM(1, 5);
	m_Def = RANDOM_NUM(1, 3);
	m_Speed = RANDOM_NUM(10, 50);
}

CUnit::~CUnit()
{
}

void CUnit::EnemyTageting(WarpList<CUnit>& Enemy)
{
	float length{ 10000 };

	for (auto p = (&Enemy)->begin(); p != (&Enemy)->end(); p = p->GetNext())
	{
		//if (p == nullptr) break;
		POINT Enemy_Pos = p->m_Elem.Get_UnitStatus_Pos();
		if ( length > GetLength(m_UnitPos.x, m_UnitPos.y, Enemy_Pos.x, Enemy_Pos.y))
		{
			length = GetLength(m_UnitPos.x, m_UnitPos.y, Enemy_Pos.x, Enemy_Pos.y);
			m_ClosedEnemy = &(p->m_Elem);
		}
	}
}

void CUnit::Move(float fTimeElapsed)
{
	m_UnitPos.x += 10.0f;
	if (m_UnitPos.x > CLIENT_WIDTH) m_UnitPos.x = 0.0f;
	//POINT Enemy_Pos = m_ClosedEnemy->Get_UnitStatus_Pos();
	//
	//MoveDirection.x = (Enemy_Pos.x - m_UnitPos.x) / GetLength(m_UnitPos.x, m_UnitPos.y, Enemy_Pos.x, Enemy_Pos.y);
	//MoveDirection.y = (Enemy_Pos.y - m_UnitPos.y) / GetLength(m_UnitPos.x, m_UnitPos.y, Enemy_Pos.x, Enemy_Pos.y);
	//
	//if (GetLength(m_UnitPos.x, m_UnitPos.y, Enemy_Pos.x, Enemy_Pos.y) > 30)
	//{
	//	MoveLength.x += MoveDirection.x * m_Speed * fTimeElapsed;
	//	MoveLength.y += MoveDirection.y * m_Speed * fTimeElapsed;
	//
	//	if (MoveLength.x >= 1)
	//	{
	//		m_UnitPos.x++;
	//		MoveLength.x = 0;
	//	}
	//	if (MoveLength.y >= 1)
	//	{
	//		m_UnitPos.y++;
	//		MoveLength.y = 0;
	//	}
	//	if (MoveLength.x <= -1)
	//	{
	//		m_UnitPos.x--;
	//		MoveLength.x = 0;
	//	}
	//	if (MoveLength.y <= -1)
	//	{
	//		m_UnitPos.y--;
	//		MoveLength.y = 0;
	//	}
	//}
}

void CUnit::Attack()
{
	POINT Enemy_Pos = m_ClosedEnemy->Get_UnitStatus_Pos();

	if (GetLength(m_UnitPos.x, m_UnitPos.y, Enemy_Pos.x, Enemy_Pos.y) <= 30)
	{
		int damege = m_Atk - m_ClosedEnemy->Get_UnitStatus_Def();
		if (damege < 0) damege = 0;

		if (*(m_ClosedEnemy->Get_UnitStatus_HP()) <= 0) return;
		*(m_ClosedEnemy->Get_UnitStatus_HP()) -= damege;

		if (*(m_ClosedEnemy->Get_UnitStatus_HP()) <= 0)
		{
			m_EXP += EXP_Coefficient(m_ClosedEnemy->Get_UnitStatus_Level());

			if (m_EXP >= LEVEL_UP_Coefficient(m_Level))

			{
				m_EXP -= LEVEL_UP_Coefficient(m_Level);
				m_Level++;
				m_hp += RANDOM_NUM(5, 20);
				m_mp += 5;
				m_Atk += RANDOM_NUM(0, 2);
				m_Speed += 10;
			}
			m_ClosedEnemy = nullptr;
		}
	}
}

void CUnit::Update(float fTimeElapsed, WarpList<CUnit>&Enemy)
{
	EnemyTageting(Enemy);

	Move(fTimeElapsed);

	m_CreateUnit_Counter += fTimeElapsed;
	if (m_CreateUnit_Counter > 1)
	{
		m_CreateUnit_Counter = 0;
		Attack();
	}
}

void CUnit::Draw(ID2D1HwndRenderTarget* pRendertarget, bool Player_Color)
{
	//swprintf_s(m_OutPut_Parameter, L"Lv: %d, HP: %d", m_Level, m_hp);
	ComPtr<ID2D1SolidColorBrush> rbrush;
	ComPtr<ID2D1SolidColorBrush> bbrush;
	pRendertarget->CreateSolidColorBrush(ColorF{ ColorF::Red }, &rbrush);
	pRendertarget->CreateSolidColorBrush(ColorF{ ColorF::Blue}, &bbrush);
	if (Player_Color)
	{
		//SetTextColor(hdc, RGB(0, 0, 255));
		//TextOut(hdc, m_UnitPos.x - 30, m_UnitPos.y - 15, m_OutPut_Parameter,lstrlen(m_OutPut_Parameter));
		//SelectObject(hdc, (HBRUSH)GetStockObject(BLACK_BRUSH));
		pRendertarget->DrawRectangle(RectF(m_UnitPos.x, m_UnitPos.y, m_UnitPos.x + m_UnitLength.x, m_UnitPos.y + m_UnitLength.y), bbrush.Get());
	}
	else
	{
		//SetTextColor(hdc, RGB(255, 0, 0));
		//TextOut(hdc, m_UnitPos.x - 30, m_UnitPos.y - 15, m_OutPut_Parameter, lstrlen(m_OutPut_Parameter));
		//SelectObject(hdc, (HBRUSH)GetStockObject(GRAY_BRUSH));
		pRendertarget->DrawRectangle(RectF(m_UnitPos.x, m_UnitPos.y, m_UnitPos.x + m_UnitLength.x, m_UnitPos.y + m_UnitLength.y), rbrush.Get());
	}
}

ostream & operator<<(ostream & os, const CUnit & p)
{
	os << "Name: " << p.m_name << " // Level: " << p.m_Level << " // HP: " << p.m_hp << " // MP: " << p.m_mp;
	return os;
}
