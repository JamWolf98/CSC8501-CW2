#include <iostream>
#include <fstream>
#include "Menu.h"
#include "Maze.h"
#include "MazeGenerator.h"

int Menu::getMenuChoice() {
	int choice = 0;

	std::cout << "\n1. Generate new maze series\n2. Load existing maze series\n3. Maze analysis\n4. Exit\n\n";

	std::cout << "Enter option: ";
	std::cin >> choice;

	if (integerInputted()) {
		switch (choice) {
		case 1:
		case 2:
		case 3:
		case 4:
			return choice;
		}
	}
	
	return getMenuChoice();
}

void Menu::generateNewMaze() {
	std::cout << "MAZE WIDTH\n";
	int sizeX = getMazeSize();
	std::cout << "\nMAZE HEIGHT\n";
	int sizeY = getMazeSize();

	std::cout << "\nNUMBER OF PLAYERS";
	int maxPlayerCount = ((round((double)sizeX / 2) - 1) * 2) + ((round((double)sizeY / 2) - 1) * 2);
	std::cout << "\nMaximum number of players/entrances for this size is " << maxPlayerCount << ".\n";
	int playerCount = getPlayerCount(maxPlayerCount);
	
	std::vector<Maze*> mazes = MazeGenerator::generateMaze(sizeX, sizeY, playerCount, true);
	MazeGenerator::displayMaze(mazes[mazes.size() - 1]);

	switch (mazes[mazes.size() - 1]->solvable) {
	case Solvable::Fully:
		std::cout << "Maze is fully solvable.\n\n";
		break;
	case Solvable::Partially:
		std::cout << "Maze is partially solvable.\n\n";
		break;
	case Solvable::Not:
		std::cout << "Maze is not solvable.\n\n";
	}

	if (saveRequested()) {
		bool successful = false;

		do {
			successful = MazeGenerator::saveMaze(mazes,
				[]() -> std::string {
					std::string name = "";

					std::cout << "\nEnter name of new maze series: ";
					std::cin >> name;

					return name;
				}()
			);
		} while (!successful);
	}

	for (auto maze : mazes) {
		delete maze;
	}
}

void Menu::loadExistingMaze() {
	std::string name = getExistingName();

	if (!name.empty()) {
		std::vector<Maze*> mazes = MazeGenerator::loadMaze(name);
		std::cin.ignore();

		for (auto maze : mazes) {
			if (MazeGenerator::validMaze(maze)) {
				MazeGenerator::displayMaze(maze);

				std::cout << "Press ENTER to continue.";
				std::cin.ignore();
			}
			else {
				std::cout << "Maze not valid.\n";
			}

			delete maze;
		}
	}
}

void Menu::mazeAnalysis() {
	std::cout << "This analysis will display the average number of steps taken to fully solve a maze as a percentage of its area for each combination of dimensions and their possible quantities of players. As will be shown at the end of the analysis, mazes are always fully solvable.\n";
	std::cout << "From the data, the effect of increasing player count on number of steps taken can be observed for each maze size.\n";
	std::cout << "\nMAXIMUM MAZE WIDTH TO ANALYSE\n";
	int sizeX = getMazeSize();
	std::cout << "\nMAXIMUM MAZE HEIGHT TO ANALYSE\n";
	int sizeY = getMazeSize();

	std::cout << "\nMAXIMUM NUMBER OF PLAYERS TO ANALYSE";
	int maxPlayerCount = ((round((double)sizeX / 2) - 1) * 2) + ((round((double)sizeY / 2) - 1) * 2);
	std::cout << "\nMaximum number of players/entrances for this size is " << maxPlayerCount << ".\n";
	int playerCount = getPlayerCount(maxPlayerCount);

	int fullySolvableCount = 0;
	int partiallySolvableCount = 0;
	int unsolvableCount = 0;
	
	for (int sX = Maze::minSize; sX <= sizeX; sX++) {
		for (int sY = Maze::minSize; sY <= sizeY; sY++) {
			std::cout << '\n' << sX << "x" << sY << '\n';

			for (int p = 2; p <= playerCount; p++) {
				if (p <= (((round((double)sX / 2) - 1) * 2) + ((round((double)sY / 2) - 1) * 2))) {
					double averageStepsPerc = 0;

					for (int i = 0; i < iter; i++) {
						std::vector<Maze*> mazes = MazeGenerator::generateMaze(sX, sY, p, false);
						Maze* maze = mazes[mazes.size() - 1];

						switch (maze->solvable) {
						case Solvable::Fully:
							fullySolvableCount++;
							break;
						case Solvable::Partially:
							partiallySolvableCount++;
							break;
						case Solvable::Not:
							unsolvableCount++;
						}

						averageStepsPerc += (double)(maze->solveSteps) / ((double)sizeX * sizeY);

						for (auto maze : mazes) {
							delete maze;
						}				
					}					

					if (averageStepsPerc != 0) {
						averageStepsPerc = averageStepsPerc / iter * 100;
						std::cout << p << ": " << averageStepsPerc << "%\n";
					}
				}
			}
		}
	}

	std::cout << "\nFully solvable mazes: " << fullySolvableCount;
	std::cout << "\nPartially solvable mazes: " << partiallySolvableCount;
	std::cout << "\nUnsolvable mazes: " << unsolvableCount << '\n';
}

int Menu::getMazeSize() {
	int size = 0;
	bool validInput = true;

	std::cout << "Enter maze side length: ";
	std::cin >> size;

	validInput = integerInputted();

	if (size < Maze::minSize) {
		std::cout << "Maze side length must be at least " << Maze::minSize << ".\n";
		validInput = false;
	}

	if (validInput) {
		return size;
	}

	return getMazeSize();
}

int Menu::getPlayerCount(int maxPlayerCount) {
	int playerCount = 0;
	bool validInput = true;
	std::cout << "Enter number of player/entrances: ";
	std::cin >> playerCount;

	validInput = integerInputted();

	if (playerCount < Maze::minPlayers) {
		std::cout << "Maze must have at least " << Maze::minPlayers << " players/entrances.\n";
		validInput = false;
	}

	if (playerCount > maxPlayerCount) {
		std::cout << "Maximum number of players/entrances for this size is " << maxPlayerCount << ".\n";
		validInput = false;
	}

	if (validInput) {
		return playerCount;
	}

	return getPlayerCount(maxPlayerCount);
}

bool Menu::saveRequested() {
	std::string save = "";

	std::cout << "Would you like to save this maze series?: ";
	std::cin >> save;

	for (auto& letter : save) {
		letter = std::tolower(letter);
	}

	if (save == "yes") {
		return true;
	}

	if (save == "no") {
		return false;
	}

	std::cout << "Invalid input.\n";

	return saveRequested();
}

std::string Menu::getExistingName() {
	std::string name = "";

	std::cout << "Enter name of existing maze series: ";
	std::cin >> name;

	std::ifstream file(name + ".txt");

	if (file.fail()) {
		std::cout << "No maze found with that name.\n";
		name.clear();
	}

	file.close();

	return name;
}

bool Menu::integerInputted() {
	if (std::cin.fail()) {
		std::cout << "Input must be an integer.\n";
		std::cin.clear();
		std::cin.ignore(1024, '\n');
		return false;
	}

	return true;
}

int Menu::runMenu() {
	int choice = getMenuChoice();

	std::cout << '\n';

	switch (choice) {
	case 1:
		generateNewMaze();
		break;
	case 2:
		loadExistingMaze();
		break;
	case 3:
		mazeAnalysis();
		break;
	case 4:
		return 0;;
	}

	return runMenu();
}