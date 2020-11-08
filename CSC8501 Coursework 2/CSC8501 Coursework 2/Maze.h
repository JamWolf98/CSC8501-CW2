#pragma once
#include <vector>
#include <string>

struct MazeTile {
	char contents = ' ';
	bool entrance = false;
	bool visited = false;
	int x;
	int y;
	int bestLength;
	MazeTile* bestParent;
};

enum class Solvable {
	Fully,
	Partially,
	Not
};

class Maze {
public:
	Maze(int sizeX, int sizeY, int entranceCount);
	Maze(Maze* maze);
	~Maze();

	std::vector<MazeTile*> getNeighbours(MazeTile* tile);

	int sizeX;
	int sizeY;
	int entranceCount;
	std::vector<std::vector<MazeTile*>> tiles;
	Solvable solvable;
	int solveSteps;

	static const int minSize = 5;
	static const int minPlayers = 2;
};