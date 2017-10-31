#include "stdafx.h"
#include "Board.h"

Board::Board() : iHeuristic(0), nRaws(0)
{
}
Board::Board(int size) : iHeuristic(0), nRaws(size)
{
	pRaws = new int[size];
	memset(pRaws, INVALID_VALUE, sizeof(int)*nRaws);
	CalcHeuristic();
}
Board::Board(const Board & other) 
	: nRaws(other.nRaws)
	, iHeuristic(other.iHeuristic)
{
	if(!pRaws) pRaws = new int[nRaws];
	memcpy(pRaws, other.pRaws, sizeof(int)*nRaws);
}
Board::Board(Board && other) noexcept
	: pRaws(other.pRaws)
	, nRaws(other.nRaws)
	, iHeuristic(other.iHeuristic)
{
	delete[] other.pRaws;
}
Board::~Board()
{
	delete[] pRaws;
}

void Board::CalcHeuristic()
{
	iHeuristic = 0;
	int colm_count = 0;
	for (int i = 0; i < nRaws; ++i)
	{
		if (pRaws[i] == INVALID_VALUE)
			iHeuristic += 5;

		for (int j = i + 1; j < nRaws; ++j)
		{
			if (i == j) continue;
			if (pRaws[i] == pRaws[j])
				++iHeuristic;
			if(abs(i-j) == abs(pRaws[i] - pRaws[j]))
				++iHeuristic;
		}
	}
}

void Board::PrintBoard()
{
	printf("\n");
	for (int i = 0; i < nRaws; ++i)
	{
		for (int j = 0; j < nRaws; ++j)
		{
			if (pRaws[i] == j)
				printf("б┌");
			else
				printf("бр");
		}
		printf("\n");
	}
}
