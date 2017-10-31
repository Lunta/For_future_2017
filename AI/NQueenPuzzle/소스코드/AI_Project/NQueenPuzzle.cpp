#include "stdafx.h"
#include "Board.h"
#include "NQueenPuzzle.h"
#include "PriorityQueue.h"
#include "GameTimer.h"

NQueenPuzzle::NQueenPuzzle()
{
	Timer.Init();
}
NQueenPuzzle::~NQueenPuzzle()
{
}

void NQueenPuzzle::ProcessAstar()
{
	int board_size = 0;
	printf("Board_Size: ");
	scanf_s("%d", &board_size);
	while (char c = getchar() != '\n');
	if (board_size < 1) return;

	printf("Ready to Astar...\n");
	std::vector<Board*> AstarGraph;
	AstarGraph.reserve(MAX_SIZE*board_size*board_size);
	const int graph_size = AstarGraph.capacity();
	int process_count = 0;

	std::vector<double> GCosts(graph_size, 0.0);
	std::vector<double> FCosts(graph_size, 0.0);

	IndexedPriorityQLow<double> pq(FCosts, graph_size);
	printf("Start to Astar Search.\n");
	Timer.GetTick();
	pq.insert(0);

	AstarGraph.push_back(new Board(board_size));

	while (!pq.empty())
	{
		int NextClosestNode = pq.Pop();

		Board* currNode = AstarGraph[NextClosestNode];

		//currNode->PrintBoard();
		if (currNode->iHeuristic == 0) {
			Timer.GetTick();
			currNode->PrintBoard();
			printf("heuristic: %d\n", currNode->iHeuristic);
			printf("process_count: %d\n", process_count);
			printf("process_time: %lf\n", Timer.GetTimeElapsed());
			break;
		}
		//printf("heuristic: %d\n", currNode->iHeuristic);
		++process_count;

		for (int i = 0; i < currNode->nRaws; ++i)
		{
			if (currNode->pRaws[i] == INVALID_VALUE)
			{
				for (int j = 0; j < currNode->nRaws; ++j)
				{
					Board* newNode = new Board(*currNode);
					newNode->pRaws[i] = j;
					newNode->CalcHeuristic();

					AstarGraph.push_back(newNode);
					int next_idx = AstarGraph.size() - 1;

					double HCost = newNode->iHeuristic;
					double GCost = GCosts[NextClosestNode] + 1;

					FCosts[next_idx] = GCost + HCost;
					GCosts[next_idx] = GCost;

					pq.insert(next_idx);
				}
				break;
			}
		}
	}
	for (auto& p : AstarGraph)
		delete p;
}

void NQueenPuzzle::ProcessHillClimbing()
{
	int board_size = 0;
	printf("Board_Size: ");
	scanf_s("%d", &board_size);
	while (char c = getchar() != '\n');
	if (board_size < 1) return;

	printf("Ready to Hill-climbing...\n");
	Board currBoard(board_size);
	int process_count = 0;

	std::vector<Board> HillClimbingNeighbors;
	std::vector<int> SmallestHeuristicIdxes;
	HillClimbingNeighbors.reserve(board_size*(board_size - 1));
	SmallestHeuristicIdxes.reserve(100);

	printf("Start to Hill-climbing.\n");
	Timer.GetTick();

	for (int i = 0; i < currBoard.nRaws; ++i)
		currBoard.pRaws[i] = rand() % currBoard.nRaws;
	currBoard.CalcHeuristic();
	while (true)
	{
		//currBoard.PrintBoard();
		if (currBoard.iHeuristic == 0) {
			Timer.GetTick();
			currBoard.PrintBoard();
			printf("heuristic: %d\n", currBoard.iHeuristic);
			printf("process_count: %d\n", process_count);
			printf("process_time: %lf\n", Timer.GetTimeElapsed());
			return;
		}
		//printf("heuristic: %d\n", currBoard.iHeuristic);
		++process_count;

		int smallest_H = currBoard.iHeuristic;
		for (int i = 0; i < currBoard.nRaws; ++i)
		{
			for (int j = 0; j < currBoard.nRaws; ++j)
			{
				if (currBoard.pRaws[i] == j) continue;
				Board neighbor(currBoard);
				neighbor.pRaws[i] = j;
				neighbor.CalcHeuristic();
				HillClimbingNeighbors.emplace_back(neighbor);
				if (neighbor.iHeuristic < smallest_H)
					smallest_H = neighbor.iHeuristic;
			}
		}

		for (int i = 0; i < HillClimbingNeighbors.size(); ++i)
			if (HillClimbingNeighbors[i].iHeuristic == smallest_H)
				SmallestHeuristicIdxes.push_back(i);

		currBoard = HillClimbingNeighbors[
			SmallestHeuristicIdxes[rand() % SmallestHeuristicIdxes.size()]];

		HillClimbingNeighbors.clear();
		SmallestHeuristicIdxes.clear();
	}
}

void NQueenPuzzle::ProcessRandomRestartHillClimbing()
{
	int board_size = 0;
	printf("Board_Size: ");
	scanf_s("%d", &board_size);
	while (char c = getchar() != '\n');
	if (board_size < 1) return;
	
	printf("Ready to Random restart hill-climbing...\n");
	Board currBoard(board_size);
	constexpr int MAX_SIDEWAY_MOVE = 30;
	int sideway_move = 0;
	int current_Heuristic = 0;
	int process_count = 0;

	std::vector<Board> HillClimbingNeighbors;
	std::vector<int> SmallestHeuristicIdxes;
	HillClimbingNeighbors.reserve(board_size*(board_size - 1));
	SmallestHeuristicIdxes.reserve(100);

	printf("Start to Random restart hill-climbing.\n");
	Timer.GetTick();

	for (int i = 0; i < currBoard.nRaws; ++i)
		currBoard.pRaws[i] = rand() % currBoard.nRaws;
	currBoard.CalcHeuristic();

	while (true)
	{
		//currBoard.PrintBoard();
		if (currBoard.iHeuristic == 0) {
			Timer.GetTick();
			currBoard.PrintBoard();
			printf("heuristic: %d\n", currBoard.iHeuristic);
			printf("process_count: %d\n", process_count);
			printf("process_time: %lf\n", Timer.GetTimeElapsed());
			return;
		}
		//printf("heuristic: %d\n", currBoard.iHeuristic);
		++process_count;

		int smallest_H = currBoard.iHeuristic;
		if (current_Heuristic == currBoard.iHeuristic)
		{
			sideway_move++;
			if(sideway_move > MAX_SIDEWAY_MOVE)
			{ 
				for (int i = 0; i < currBoard.nRaws; ++i)
					currBoard.pRaws[i] = rand() % currBoard.nRaws;
				currBoard.CalcHeuristic();
				sideway_move = 0;
				HillClimbingNeighbors.clear();
				SmallestHeuristicIdxes.clear();
				continue;
			}
		}
		else
		{
			current_Heuristic = currBoard.iHeuristic;
			sideway_move = 0;
		}

		for (int i = 0; i < currBoard.nRaws; ++i)
		{
			for (int j = 0; j < currBoard.nRaws; ++j)
			{
				if (currBoard.pRaws[i] == j) continue;
				Board neighbor(currBoard);
				neighbor.pRaws[i] = j;
				neighbor.CalcHeuristic();
				HillClimbingNeighbors.emplace_back(neighbor);
				if (neighbor.iHeuristic < smallest_H)
					smallest_H = neighbor.iHeuristic;
			}
		}

		for (int i = 0; i < HillClimbingNeighbors.size(); ++i)
			if (HillClimbingNeighbors[i].iHeuristic == smallest_H)
				SmallestHeuristicIdxes.push_back(i);

		currBoard = HillClimbingNeighbors[
			SmallestHeuristicIdxes[rand() % SmallestHeuristicIdxes.size()]];

		HillClimbingNeighbors.clear();
		SmallestHeuristicIdxes.clear();
	}
}