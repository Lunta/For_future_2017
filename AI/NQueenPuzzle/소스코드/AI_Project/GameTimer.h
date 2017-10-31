#pragma once
#include <chrono>

class GameTimer final
{
private:
	std::chrono::system_clock::time_point	m_tpCurrentTime;
	std::chrono::duration<double>			m_dTimeElapsed;
	double									m_dFps;

private:
	GameTimer();

public:
	~GameTimer();

	void Init();
	bool GetTick();
	double GetTimeElapsed() { return m_dTimeElapsed.count(); };
	int GetFPS() { return (int)m_dFps; };

public:
	static GameTimer& GetInstance();
};

static GameTimer& Timer = GameTimer::GetInstance();