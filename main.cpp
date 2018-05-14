//////////////////////////////
// main.cpp                 //
// Andrew Krepps            //
// EN.605.417 Final Project //
// 15 May 2018              //
//////////////////////////////

#include "GeneticSolver.h"
#include "HostSolver.h"
#include "Path.h"
#include "Puzzle.h"
#include "PuzzleReader.h"
#include "Solver.h"

#include <chrono>
#include <iostream>
#include <string>

void runSolver(gws::Solver* solver, const std::string& name, const gws::Puzzle& puzzle, gws::Path& path)
{
	auto start = std::chrono::high_resolution_clock::now();
	bool solutionFound = solver->solvePuzzle(puzzle, path);
	auto stop = std::chrono::high_resolution_clock::now();
	float hostMs = std::chrono::duration<float>(stop - start).count()*1000.0f;
	
	std::cout << std::endl;
	if (solutionFound) {
		std::cout << "Puzzle solved on " << name << std::endl;
		std::cout << "Starting row: " << puzzle.getPointRow(path.getStartPointIndex()) << " | Starting col: " << puzzle.getPointCol(path.getStartPointIndex()) << std::endl;
		std::cout << "Path: " << path << std::endl;
	}
	else {
		std::cout << "No puzzle solution found on " << name << std::endl;
	}
	std::cout << name << " execution time: " << hostMs << " ms" << std::endl;
}

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
	
	std::cout << "Reading puzzle from " << puzzleFile << std::endl;
	gws::Puzzle puzzle = gws::readPuzzle(puzzleFile, &pointData, &edgeData, &spaceData);
	
	// each point can only be visited once, so number of points can be used as the max path length
	size_t maxPathLength = puzzle.getNumPoints();
	char* moveData = new char[maxPathLength];
	gws::Path path(moveData, maxPathLength);
	
	// solve puzzle and display timing metrics
	gws::HostSolver* hostSolver = new gws::HostSolver();
	if (puzzle.getWidth() < 7 || puzzle.getHeight() < 7) {
		runSolver(hostSolver, "CPU", puzzle, path);
		std::cout << "CPU solution evaluations: " << puzzle.getNumEvals() << std::endl;
	}
	else {
		std::cout << "Puzzle is too large to solve on host" << std::endl;
	}
	
	gws::GeneticSolver* gpuSolver = new gws::GeneticSolver(puzzle.getWidth(), puzzle.getHeight(), 8192, 1000000, 0.75, 0.1, 0);
	runSolver(gpuSolver, "GPU", puzzle, path);
	std::cout << "GPU population generations: " << gpuSolver->getNumIterations() << std::endl;
	
	// clean up memory
	delete hostSolver;
	delete gpuSolver;
	
	delete [] pointData;
	delete [] edgeData;
	delete [] spaceData;
	delete [] moveData;
	
	return EXIT_SUCCESS;
}