#pragma once
struct Node {
	int height;
	int queenWidth;

	Node() : height(0), queenWidth(INVALID_VALUE) {}
	Node(int y) : height(y), queenWidth(INVALID_VALUE) {}
};

class Graph
{
public:
	Node*	m_pBuf;
	int		m_iSize;
	int		m_Heuristic;

public:
	Graph() = delete;
	Graph(int size);
	~Graph();

	void Clear();

	void CalcAstarHeuristic();

};