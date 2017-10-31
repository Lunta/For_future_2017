#include "stdafx.h"
#include "Graph.h"

Graph::Graph(int size)
	:m_iSize(size)
	, m_Heuristic(50 * size)
{
	m_pBuf = new Node[m_iSize];
	for (int i = 0; i < m_iSize; ++i)
	{
		m_pBuf[i].queenWidth = INVALID_VALUE;
		m_pBuf[i].height = i;
	}
}
Graph::~Graph()
{
	if (m_pBuf) delete[] m_pBuf;
}

void Graph::Clear()
{
	for (int i = 0; i < m_iSize; ++i)
	{
		m_pBuf[i].height = i;
		m_pBuf[i].queenWidth = INVALID_VALUE;
	}
}

void Graph::CalcAstarHeuristic()
{
	int h = 0;
	for (int i = 0; i < m_iSize; ++i)
	{
		for (int j = 0; j < m_iSize; ++j)
		{
			if (i == j) continue;
			if (m_pBuf[i].queenWidth == m_pBuf[j].queenWidth)
				h++;
			if (abs(i-j) == abs(m_pBuf[i].queenWidth - m_pBuf[j].queenWidth))
				h++;
		}
	}
	m_Heuristic = h;
}
