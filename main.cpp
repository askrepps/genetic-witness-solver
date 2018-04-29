#include "Puzzle.h"
#include "PuzzleReader.h"

#include <iostream>

int main(int argc, char** argv)
{
	char* pointData;
	char* edgeData;
	char* spaceData;
	gws::Puzzle puzzle = gws::readPuzzle("data/test.txt", &pointData, &edgeData, &spaceData);
	
	std::cout << "Puzzle info: " << std::endl;
	std::cout << "Num points: " << puzzle.getNumPoints() << std::endl;
	std::cout << "Num edges: " << puzzle.getNumEdges() << std::endl;
	std::cout << "Num spaces: " << puzzle.getNumSpaces() << std::endl;
	
	for (size_t row = 0; row < puzzle.getHeight(); ++row) {
		for (size_t col = 0; col < puzzle.getWidth(); ++col) {
			std::cout << "Point " << puzzle.getPointIndex(row, col) << ": " << puzzle.getPointValue(row, col) << std::endl;
		}
	}
	
	for (size_t row = 0; row < puzzle.getHeight(); ++row) {
		for (size_t col = 0; col < puzzle.getWidth(); ++col) {
			if (col < puzzle.getWidth() - 1) {
				std::cout << "Edge " << puzzle.getEdgeIndex(row, col, row, col + 1) << ": " << puzzle.getEdgeValue(row, col, row, col + 1) << std::endl;
			}
			if (row < puzzle.getHeight() - 1) {
				std::cout << "Edge " << puzzle.getEdgeIndex(row, col, row + 1, col) << ": " << puzzle.getEdgeValue(row, col, row + 1, col) << std::endl;
			}
		}
	}
	
	for (size_t row = 0; row < puzzle.getHeight() - 1; ++row) {
		for (size_t col = 0; col < puzzle.getWidth() - 1; ++col) {
			std::cout << "Space " << puzzle.getSpaceIndex(row, col) << ": " << puzzle.getSpaceValue(row, col) << std::endl;
		}
	}
	
	delete [] pointData;
	delete [] edgeData;
	delete [] spaceData;
	
	return EXIT_SUCCESS;
}