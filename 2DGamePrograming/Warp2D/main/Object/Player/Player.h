#pragma once

#include "Object/Object.h"
class CIndRes;

struct UserInfo
{
	float		maxHP;
	float		HP;

	float		atk;
	float		def;

	float		dex;
	float		agi;

	UINT		level;
	UINT		exp;

	UserInfo() : maxHP(), HP(), atk(), def(), dex(), agi(), level(), exp() {}

	bool GetExp(int exp) { this->exp += exp; return LevelUp(); }
	bool LevelUp()
	{
		if (exp < level * 100) return false;
		exp -= level * 100;
		level++;

		maxHP += 12;
		HP = maxHP;
		atk += 1.125;
		def++;
		dex += 3;
		agi += 2;

		return true;
	}

	static UserInfo GetInfoFromLevel(UINT level)
	{
		UserInfo info;
		info.level = level;
		info.exp = 0;
		info.maxHP = level * 12 + 100;
		info.HP = info.maxHP;

		info.atk = 10 + level * 1.125f;
		info.def = 5 + level;

		info.dex = level * 3;
		info.agi = 5 + level * 2;
		return info;
	}
};

class CPlayer : public CObject
{
public:
	enum Dir { left = 1, top = 3, right = 2, bottom = 0 };

	CPlayer(D2D_SIZE_U pt);
	virtual ~CPlayer() override;

	virtual void Update(float fTimeElapsed) override;
	virtual void Draw(ID2D1HwndRenderTarget* RenderTarget) override;
	virtual void DrawUI(ID2D1HwndRenderTarget* RenderTarget) override;

	virtual void RegisterImage(CIndRes* indres,
		ID2D1HwndRenderTarget* RenderTarget, path filename, D2D_SIZE_U szSprite);

	virtual void RegisterImage(const ComPtr<ID2D1Bitmap1>& bmp, D2D_SIZE_U szSprite);
	virtual void RegisterImage(ComPtr<ID2D1Bitmap1>&& bmp, D2D_SIZE_U szSprite) noexcept;

	void Move(Dir dir);

	const D2D_SIZE_U& GetCoord() const { return m_szCoord; }

private:
	ComPtr<ID2D1Bitmap1>		m_bmpImage;
	D2D_SIZE_U					m_szCoord;

	Dir							m_Direction;
	D2D_SIZE_U					m_szSprite;
	UINT						m_iSprite;

	float						m_fTick;

public:
	const UserInfo& GetInfo() const { return m_UserInfo; }
	void GetDamage(float atk)
	{
		m_UserInfo.HP = max(0, m_UserInfo.HP - max(0, atk - m_UserInfo.def));
		m_UserInfo.GetExp(rand() % 3 + 1);
	}

private:
	UserInfo					m_UserInfo;

};
