#pragma once

#define MENU_LENGTH 4

class Graph;
class NQueenPuzzle;

class ConsoleFramework
{
private:
	const int			m_nMenuLength;

	NQueenPuzzle*		m_pQPuzzle;

	void PrintMenu();
	int	InputCommand();

public:
	ConsoleFramework();
	~ConsoleFramework();

	void Initailize();
	void Run();
};

