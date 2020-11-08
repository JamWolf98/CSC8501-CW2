#pragma once
#include <string>

class Menu {
public:
	Menu() {};
	~Menu() {};

	static int runMenu();

private:
	static int getMenuChoice();
	static void generateNewMaze();
	static void loadExistingMaze();
	static void mazeAnalysis();
	static int getMazeSize();
	static int getPlayerCount(int maxPlayerCount);
	static bool saveRequested();
	static std::string getExistingName();
	static bool integerInputted();

	
	static const int iter = 100;
};
