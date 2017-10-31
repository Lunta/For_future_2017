#pragma once
class Board
{
public:
	int 				*pRaws{ nullptr };
	int					nRaws;
	int					iHeuristic;

public:
	Board();
	Board(int size);
	Board(const Board& other);
	Board(Board&& other) noexcept;
	~Board();

	void CalcHeuristic();
	void PrintBoard();

	void operator=(const Board& other) {
		nRaws = other.nRaws;
		iHeuristic = other.iHeuristic;
		memcpy(pRaws, other.pRaws, sizeof(int)*nRaws);
	}
};

