#pragma once

#define STRING_LENGTH 50
#define RANDOM_NUM(Min, Max) Min + rand() % (Max - Min + 1)
#define GetLength(x1, y1, x2, y2) sqrt( ( x2 - x1 )*( x2 - x1 ) + ( y2- y1 )*( y2 - y1 ) )
#define LEVEL_UP_Coefficient(LEVEL) LEVEL * 10
#define EXP_Coefficient(LEVEL) LEVEL * 10

using namespace std;

class CUnit
{
private:
	POINT m_UnitPos{ 0, 0 };
	POINT m_UnitLength{ 0, 0 };

	CUnit*m_ClosedEnemy{ nullptr };
	Vector2D<float> MoveDirection{ 0, 0 };
	Vector2D<float> MoveLength{ 0, 0 };

	TCHAR m_name[STRING_LENGTH]{ 0 };
	int m_Level{ 0 };
	int m_EXP{ 0 };
	int m_hp{ 0 };
	int m_mp{ 0 };
	int m_Atk{ 0 };
	int m_Def{ 0 };
	float m_Speed{ 0 };

	TCHAR m_OutPut_Parameter[STRING_LENGTH]{ 0 };
	float m_CreateUnit_Counter{ 0 };

public:
	CUnit();
	CUnit(RECT rc);

	~CUnit();
	POINT Get_UnitStatus_Pos() const { return m_UnitPos; }
	int Get_UnitStatus_Level() const { return m_Level; }
	int Get_UnitStatus_Atk() const { return m_Atk; }
	int Get_UnitStatus_Def() const { return m_Def; }
	int Get_UnitStatus_HP() const { return m_hp; }
	int* Get_UnitStatus_HP() { return &m_hp; }

	void EnemyTageting(WarpList<CUnit>&Enemy);
	void Move(float fTimeElapsed);
	void Attack();

	void Update(float fTimeElapsed, WarpList<CUnit>&Enemy);

	void Draw(ID2D1HwndRenderTarget* pRendertarget, bool Player_Color);

	friend ostream& operator<< (ostream& os, const CUnit& p);
};

