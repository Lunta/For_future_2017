#include "stdafx.h"
#include "Graph.h"
#include "NQueenPuzzle.h"
#include "ConsoleFramework.h"


void ConsoleFramework::PrintMenu()
{
	printf("=============Menu=============\n");
	printf("1. A* \n");
	printf("2. Hill-climbing\n");
	printf("3. Random restart Hill-climbing\n");
	printf("4. Exit\n");
}
int ConsoleFramework::InputCommand()
{
	int iCommand = 0;
	printf("-Command:");
	scanf_s("%d", &iCommand);
	while (char c = getchar() != '\n');
	if (iCommand < 1 || iCommand > m_nMenuLength)
		iCommand = 0;
	return iCommand;
}

ConsoleFramework::ConsoleFramework() 
	: m_nMenuLength(MENU_LENGTH)
{
}
ConsoleFramework::~ConsoleFramework()
{
	if (m_pQPuzzle) delete m_pQPuzzle;
}

void ConsoleFramework::Initailize()
{
	srand((unsigned int)time(NULL));
	m_pQPuzzle = new NQueenPuzzle();
}

void ConsoleFramework::Run()
{
	bool bRun = true;
	while (bRun)
	{
		PrintMenu();
		switch(InputCommand())
		{
		case 1: m_pQPuzzle->ProcessAstar(); break;
		case 2: m_pQPuzzle->ProcessHillClimbing(); break;
		case 3: m_pQPuzzle->ProcessRandomRestartHillClimbing(); break;
		case 4: 
			bRun = false; 
			continue; 
		default:
			system("cls");
			continue;
		}
	}
}
