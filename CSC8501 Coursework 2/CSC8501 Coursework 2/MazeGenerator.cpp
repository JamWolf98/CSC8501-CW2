#include <iostream>
#include <fstream>
#include "MazeGenerator.h"
#include "Maze.h"

std::vector<Maze*> MazeGenerator::generateMaze(int sizeX, int sizeY, int entranceCount, bool display) {
	Maze* maze = new Maze(sizeX, sizeY, entranceCount);

	int centreX = maze->sizeX / 2;
	int centreY = maze->sizeY / 2;

	for (int i = 1; i < maze->sizeX - 1; i += 2) {
		for (int j = 1; j < maze->sizeY - 1; j += 2) {
			int r = rand() % 2;

			if (r == 0 && i != 1) {
				maze->tiles[i][j - 1]->contents = 'X';
			}
			else if (r == 1 && j != 1) {
				maze->tiles[i - 1][j]->contents = 'X';
			}
		}
	}

	for (int i = centreX - 1; i < centreX + 2; i++) {
		for (int j = centreY - 1; j < centreY + 2; j++) {
			maze->tiles[i][j]->contents = ' ';
		}
	}

	maze->tiles[centreX][centreY]->contents = 'F';

	std::vector<MazeTile*> borderTiles;

	for (int i = 0; i < sizeX; i++) {
		maze->tiles[i][0]->contents = 'X';
		maze->tiles[i][sizeY - 1]->contents = 'X';

		borderTiles.emplace_back(maze->tiles[i][0]);
		borderTiles.emplace_back(maze->tiles[i][sizeY - 1]);
	}

	for (int i = 0; i < sizeY; i++) {
		maze->tiles[0][i]->contents = 'X';
		maze->tiles[sizeX - 1][i]->contents = 'X';

		borderTiles.emplace_back(maze->tiles[0][i]);
		borderTiles.emplace_back(maze->tiles[sizeX - 1][i]);
	}

	std::vector<MazeTile*> entrances;

	for (int i = 0; i < maze->entranceCount; i++) {
		bool successful;
		MazeTile* entrance = generateEntrance(maze, borderTiles, &successful);

		if (successful) {
			entrances.emplace_back(entrance);
			entrance->entrance = true;
		}
		else if (display) {
			std::cout << "\nCould not place an entrance.";
		}
	}
	
	if (display) {
		std::cin.ignore();
	}
	
	std::vector<std::vector<MazeTile*>> routes;
	
	for (auto& en : entrances) {
		en->contents = 'P';
		std::vector<MazeTile*> route = generateRoute(maze, en);

		if (!route.empty()) {
			routes.emplace_back(route);
		}
	}

	std::vector<Maze*> mazes;
	mazes.emplace_back(new Maze(maze));

	while (!routes.empty()) {
		if (display) {
			displayMaze(maze);

			std::cout << "Press ENTER to continue.";
			std::cin.ignore();
		}

		maze->solveSteps++;

		if (!movePlayers(maze, routes)) {
			break;
		}

		mazes.emplace_back(new Maze(maze));
	}

	delete maze;
	return mazes;
}

MazeTile* MazeGenerator::generateEntrance(Maze* maze, std::vector<MazeTile*> borderTiles, bool* successful) {
	int x = rand() % borderTiles.size();
	auto a = std::find(borderTiles.begin(), borderTiles.end(), borderTiles[x]);
	MazeTile* tile = borderTiles[x];

	borderTiles.erase(a);

	std::vector<MazeTile*> neighbours = maze->getNeighbours(tile);

	bool validEntrance = false;

	for (auto n : neighbours) {
		if (n->contents == ' ') {
			validEntrance = true;
		}
		else if (n->contents == 'E') {
			validEntrance = false;
			break;
		}
	}

	if (neighbours.size() != 3) {
		validEntrance = false;
	}

	if (tile->contents == 'E') {
		validEntrance = false;
	}

	if (validEntrance) {
		tile->contents = 'E';
		*successful = true;
		return tile;
	}

	if (borderTiles.empty()) {
		*successful = false;
		return tile;
	}

	return generateEntrance(maze, borderTiles, successful);
}

std::vector<MazeTile*> MazeGenerator::generateRoute(Maze* maze, MazeTile* entrance) {
	bool entranceFound = false;
	int centreX = maze->sizeX / 2;
	int centreY = maze->sizeY / 2;

	std::vector<MazeTile*> nextTiles;
	maze->tiles[centreX][centreY]->bestLength = 0;
	nextTiles.emplace_back(maze->tiles[centreX][centreY]);
	MazeTile* currentTile;

	do {
		currentTile = nextTiles.front();
		currentTile->visited = true;

		if (currentTile == entrance) {
			entranceFound = true;
		}
		else {
			std::vector<MazeTile*> neighbours = maze->getNeighbours(currentTile);

			for (auto& n : neighbours) {
				if (!n->visited && n->contents != 'X' && !(n->contents == 'E' && n != entrance) && currentTile->bestLength < n->bestParent->bestLength) {
					n->bestParent = currentTile;
					n->bestLength = n->bestParent->bestLength + 1;
					nextTiles.emplace_back(n);
				}
			}

			auto a = std::find(nextTiles.begin(), nextTiles.end(), currentTile);

			if (a != nextTiles.end()) {
				nextTiles.erase(a);
			}
		}
	} while (!entranceFound && !nextTiles.empty());

	std::vector<MazeTile*> route;

	if (entranceFound) {
		MazeTile* routeTile = currentTile;
		route.emplace_back(routeTile);

		while (routeTile != maze->tiles[centreX][centreY]) {
			routeTile = routeTile->bestParent;
			route.emplace_back(routeTile);
		}
	}

	for (int i = 0; i < maze->sizeX; i++) {
		for (int j = 0; j < maze->sizeY; j++) {
			maze->tiles[i][j]->bestParent = maze->tiles[0][0];
			maze->tiles[i][j]->bestLength = maze->sizeX * maze->sizeY;
			maze->tiles[i][j]->visited = false;
		}
	}

	return route;
}

bool MazeGenerator::movePlayers(Maze* maze, std::vector<std::vector<MazeTile*>>& routes) {
	int failedMoves = 0;

	for (auto& route : routes) {
		if (route.size() > 1) {
			if (route[1]->contents != 'P') {
				route[0]->contents = route[0]->entrance ? 'E' : ' ';
				route[1]->contents = 'P';

				route.erase(route.begin());
			}
			else {
				failedMoves++;
			}
		}
		else {
			route[0]->contents = 'F';

			route.erase(route.begin());
		}
	}

	if (failedMoves == maze->entranceCount) {
		maze->solvable = Solvable::Not;
		return false;
	}
	else if (failedMoves == routes.size() && failedMoves > 0) {
		maze->solvable = Solvable::Partially;
		return false;
	}

	for (auto i = routes.begin(); i != routes.end();) {
		if ((*i).empty()) {
			i = routes.erase(i);
		}
		else {
			++i;
		}
	}

	return true;
}

bool MazeGenerator::saveMaze(std::vector<Maze*> mazes, std::string name) {
	std::ofstream file(name + ".txt");

	if (file.fail()) {
		std::cout << "Invalid name with your file system.";

		return false;
	}
	else {
		for (auto& maze : mazes) {
			for (int j = 0; j < maze->sizeY; j++) {
				for (int i = 0; i < maze->sizeX; i++) {
					file << maze->tiles[i][j]->contents;
				}

				file << '\n';
			}

			file << '\n';
		}

		file.close();

		return true;
	}
}

std::vector<Maze*> MazeGenerator::loadMaze(std::string name) {
	std::ifstream file(name + ".txt");
	std::string line;
	
	std::getline(file, line);
	int sizeX = line.length();	
	file.seekg(0);
	int sizeY = 0;

	while (line != "") {
		std::getline(file, line);

		if (line != "") {
			sizeY++;
		}
	}

	std::vector<Maze*> mazes;
	file.seekg(0);
	std::getline(file, line);

	while (!file.eof()) {
		Maze* maze = new Maze(sizeX, sizeY, 0);
		int x;
		int y = 0;
		int entranceCount = 0;		

		do {			
			x = 0;

			for (auto& tile : line) {
				if (x % (maze->sizeX - 1) == 0 || y % (maze->sizeY - 1) == 0) {
					switch (tile) {
					case 'E':
					case 'P':
						maze->tiles[x][y]->entrance = true;
						maze->entranceCount++;
					}
				}

				maze->tiles[x][y]->contents = tile;
				x++;
			}

			y++;
			std::getline(file, line);
		} while (line != "" && !file.eof());

		mazes.emplace_back(maze);

		while (line == "" && !file.eof()) {
			std::getline(file, line);
		}	
	}

	file.close();

	return mazes;
}

bool MazeGenerator::validMaze(Maze* maze) {
	if (maze->sizeX < Maze::minSize) {
		return false;
	}

	if (maze->sizeY < Maze::minSize) {
		return false;
	}

	if (maze->entranceCount < Maze::minPlayers) {
		return false;
	}

	int centreX = maze->sizeX / 2;
	int centreY = maze->sizeY / 2;

	switch (maze->tiles[centreX][centreY]->contents) {
	case 'F':
	case 'P':
		break;
	default:
		return false;
	}

	for (int i = 0; i < maze->sizeX; i++) {
		for (int j = 0; j < maze->sizeY; j++) {
			if (i % (maze->sizeX - 1) == 0 || j % (maze->sizeY - 1) == 0) {
				switch (maze->tiles[i][j]->contents) {
				case 'X':
				case 'P':
					break;
				case 'E':
					if (generateRoute(maze, maze->tiles[i][j]).empty()) {
						return false;
					}
					break;
				default:
					return false;
				}
			} else if (i == centreX && j == centreY) {
				continue;
			} else if (abs(centreX - i) < 2 && abs(centreY - j) < 2) {
				switch (maze->tiles[i][j]->contents) {
				case ' ':
				case 'P':
					break;
				default:
					return false;
				}
			} 
			else {
				switch (maze->tiles[i][j]->contents) {
				case 'X':
				case ' ':
				case 'P':
					break;
				default:
					return false;
				}
			}
		}
	}

	return true;
}

void MazeGenerator::displayMaze(Maze* maze) {
	std::cout << "\n\n";

	for (int j = 0; j < maze->sizeY; j++) {
		for (int i = 0; i < maze->sizeX; i++) {
			std::cout << maze->tiles[i][j]->contents;
		}

		std::cout << '\n';
	}

	std::cout << '\n';
}
