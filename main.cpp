#include "Puzzle.h"

#include <iostream>

int main(int argc, char** argv)
{
	const char* pointData = "sooe";
	const char* edgeData = "oxoo";
	const char* spaceData = "w";
	
	gws::Puzzle puzzle(2, 2, pointData, edgeData, spaceData);
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
	
	return EXIT_SUCCESS;
}