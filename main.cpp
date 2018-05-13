//////////////////////////////
// main.cpp                 //
// Andrew Krepps            //
// EN.605.417 Final Project //
// 15 May 2018              //
//////////////////////////////

#include "HostSolver.h"
#include "Path.h"
#include "Puzzle.h"
#include "PuzzleReader.h"
#include "Solver.h"

#include <chrono>
#include <iostream>
#include <string>

int main(int argc, char** argv)
{
	// configure run
	std::string puzzleFile = "data/test.txt";
	if (argc > 1) {
		puzzleFile = argv[1];
	}
	
	char* pointData;
	char* edgeData;
	char* spaceData;
	gws::Puzzle puzzle = gws::readPuzzle(puzzleFile, &pointData, &edgeData, &spaceData);
	
	// each point can only be visited once, so number of points can be used as the max path length
	size_t maxPathLength = puzzle.getNumPoints();
	char* moveData = new char[maxPathLength];
	gws::Path path(moveData, maxPathLength);
	
	// solve puzzle (on host for now)
	gws::Solver* solver = new gws::HostSolver();
	
	auto start = std::chrono::high_resolution_clock::now();
	bool solutionFound = solver->solvePuzzle(puzzle, path);
	auto stop = std::chrono::high_resolution_clock::now();
	float ms = std::chrono::duration<float>(stop - start).count()*1000.0f;
	
	if (solutionFound) {
		std::cout << "Puzzle solved" << std::endl;
		std::cout << "Starting row: " << puzzle.getPointRow(path.getStartPointIndex()) << " | Starting col: " << puzzle.getPointCol(path.getStartPointIndex()) << std::endl;
		std::cout << "Path: " << path << std::endl;
	}
	else {
		std::cout << "No puzzle solution found" << std::endl;
	}
	
	std::cout << "Execution time: " << ms << " ms" << std::endl;
	
	// clean up memory
	delete [] pointData;
	delete [] edgeData;
	delete [] spaceData;
	delete [] moveData;
	
	return EXIT_SUCCESS;
}