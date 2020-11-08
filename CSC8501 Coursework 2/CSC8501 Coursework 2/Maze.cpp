#include "Maze.h"

Maze::Maze(int sizeX, int sizeY, int entranceCount) : sizeX(sizeX), sizeY(sizeY), entranceCount(entranceCount), solvable(Solvable::Fully), solveSteps(0) {
	tiles.resize(sizeX);

	for (int i = 0; i < sizeX; i++) {
		tiles[i].resize(sizeY);

		for (int j = 0; j < sizeY; j++) {
			tiles[i][j] = new MazeTile();
			tiles[i][j]->bestLength = sizeX * sizeY;
			tiles[i][j]->bestParent = tiles[0][0];
			tiles[i][j]->x = i;
			tiles[i][j]->y = j;
		}
	}
}

Maze::Maze(Maze* maze) {
	sizeX = maze->sizeX;
	sizeY = maze->sizeY;
	entranceCount = maze->entranceCount;
	solvable = maze->solvable;
	solveSteps = maze->solveSteps;

	tiles.resize(sizeX);

	for (int i = 0; i < sizeX; i++) {
		tiles[i].resize(sizeY);

		for (int j = 0; j < sizeY; j++) {
			tiles[i][j] = new MazeTile();
			tiles[i][j]->bestLength = sizeX * sizeY;
			tiles[i][j]->bestParent = tiles[0][0];
			tiles[i][j]->x = i;
			tiles[i][j]->y = j;
			tiles[i][j]->contents = maze->tiles[i][j]->contents;
		}
	}
}

Maze::~Maze() {
	for (auto i : tiles) {
		for (auto j : i) {
			delete j;
		}
	}
}

std::vector<MazeTile*> Maze::getNeighbours(MazeTile* tile) {
	std::vector<MazeTile*> neighbours;

	if (tile->x > 0) {
		neighbours.emplace_back(tiles[tile->x - 1][tile->y]);
	}

	if (tile->y > 0) {
		neighbours.emplace_back(tiles[tile->x][tile->y - 1]);
	}

	if (tile->x < sizeX - 1) {
		neighbours.emplace_back(tiles[tile->x + 1][tile->y]);
	}

	if (tile->y < sizeY - 1) {
		neighbours.emplace_back(tiles[tile->x][tile->y + 1]);
	}

	return neighbours;
}
