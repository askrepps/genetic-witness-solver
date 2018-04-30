//////////////////////////////
// HostSolver.cpp           //
// Andrew Krepps            //
// EN.605.417 Final Project //
// 15 May 2018              //
//////////////////////////////

#include "HostSolver.h"

#include "Path.h"
#include "Puzzle.h"

#include <iostream>

namespace gws
{
	HostSolver::~HostSolver() {}
	
	bool HostSolver::solvePuzzle(const Puzzle& puzzle, Path& path)
	{
		bool solutionFound = false;
		
		// keep track of which points have been visited
		bool* visitFlags = new bool[puzzle.getNumPoints()];
		
		// search puzzle for starting points and start exhaustive search from
		// each one until a solution is found or there are no more starting points
		size_t row = 0;
		while (!solutionFound && row < puzzle.getHeight()) {
			size_t col = 0;
			while (!solutionFound && col < puzzle.getWidth()) {
				if (puzzle.getPointValue(row, col) == PointValue::START) {
					// reset path and visit flags for new search
					path.clear();
					path.setStartPointIndex(puzzle.getPointIndex(row, col));
					for (size_t i = 0; i < puzzle.getNumPoints(); ++i) {
						visitFlags[i] = false;
					}
					
					solutionFound = searchPuzzle(puzzle, path, row, col, visitFlags);
				}
				
				++col;
			}
			
			++row;
		}
		
		delete [] visitFlags;
		
		return solutionFound;
	}
	
	bool HostSolver::searchPuzzle(const Puzzle& puzzle, Path& path, size_t row, size_t col, bool* visitFlags)
	{
		bool solutionFound = false;
		
		// mark current point as visited
		size_t currentIndex = puzzle.getPointIndex(row, col);
		visitFlags[currentIndex] = true;
		
		// if current point is the end, evalulate solution
		if (puzzle.getPointValue(row, col) == PointValue::END) {
			solutionFound = puzzle.evaluateSolution(path);
		}
		else {
			// search all possible moves for solution (up, down, left, right)
			// valid move is one where destination edge/point are not blocked and point hasn't been visited already
			if (!solutionFound && row > 0 && puzzle.getEdgeValue(row - 1, col, row, col) != EdgeValue::BLOCKED
				  && puzzle.getPointValue(row - 1, col) != PointValue::BLOCKED && !visitFlags[puzzle.getPointIndex(row - 1, col)]) {
				path.addMove(MoveValue::UP);
				solutionFound = searchPuzzle(puzzle, path, row - 1, col, visitFlags);
			}
			if (!solutionFound && row < puzzle.getHeight() - 1 && puzzle.getEdgeValue(row, col, row + 1, col) != EdgeValue::BLOCKED
				  && puzzle.getPointValue(row + 1, col) != PointValue::BLOCKED && !visitFlags[puzzle.getPointIndex(row + 1, col)]) {
				path.addMove(MoveValue::DOWN);
				solutionFound = searchPuzzle(puzzle, path, row + 1, col, visitFlags);
			}
			if (!solutionFound && col > 0 && puzzle.getEdgeValue(row, col - 1, row, col) != EdgeValue::BLOCKED
				  && puzzle.getPointValue(row, col - 1) != PointValue::BLOCKED && !visitFlags[puzzle.getPointIndex(row, col - 1)]) {
				path.addMove(MoveValue::LEFT);
				solutionFound = searchPuzzle(puzzle, path, row, col - 1, visitFlags);
			}
			if (!solutionFound && col < puzzle.getWidth() - 1 && puzzle.getEdgeValue(row, col, row, col + 1) != EdgeValue::BLOCKED
				  && puzzle.getPointValue(row, col + 1) != PointValue::BLOCKED && !visitFlags[puzzle.getPointIndex(row, col + 1)]) {
				path.addMove(MoveValue::RIGHT);
				solutionFound = searchPuzzle(puzzle, path, row, col + 1, visitFlags);
			}
		}
		
		// remove current point from path and visit flags if we are backtracking
		if (!solutionFound) {
			path.popMove();
			visitFlags[currentIndex] = false;
		}
		
		return solutionFound;
	}
}