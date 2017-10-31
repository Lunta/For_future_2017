#pragma once

class Board;
class NQueenPuzzle
{
private:

public:
	NQueenPuzzle();
	~NQueenPuzzle();

	void ProcessAstar();
	void ProcessHillClimbing();
	void ProcessRandomRestartHillClimbing();
};

