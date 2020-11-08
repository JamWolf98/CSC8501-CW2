#pragma once
#include <vector>

struct MazeTile;
class Maze;

class MazeGenerator {
public:
	MazeGenerator() {};
	~MazeGenerator() {};

	static std::vector<Maze*> generateMaze(int sizeX, int sizeY, int entranceCount, bool display);
	static MazeTile* generateEntrance(Maze* maze, std::vector<MazeTile*> borderTiles, bool* successful);
	static std::vector<MazeTile*> generateRoute(Maze* maze, MazeTile* entrance);
	static bool movePlayers(Maze* maze, std::vector<std::vector<MazeTile*>>& routes);
	static bool saveMaze(std::vector<Maze*> mazes, std::string name);
	static std::vector<Maze*> loadMaze(std::string name);
	static bool validMaze(Maze* maze);
	static void displayMaze(Maze* maze);
};
