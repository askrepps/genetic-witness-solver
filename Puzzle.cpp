//////////////////////////////
// Puzzle.cpp               //
// Andrew Krepps            //
// EN.605.417 Final Project //
// 15 May 2018              //
//////////////////////////////

#include "Puzzle.h"

#include "Path.h"

#include <ostream>

namespace gws
{
	size_t Puzzle::getNumPoints(size_t width, size_t height)
	{
		return width*height;
	}
	
	size_t Puzzle::getNumEdges(size_t width, size_t height)
	{
		// horizontal edges + vertical edges
		return (width - 1)*height + width*(height - 1);
	}
	
	size_t Puzzle::getNumSpaces(size_t width, size_t height)
	{
		return (width - 1)*(height - 1);
	}
	
	Puzzle::Puzzle(size_t w, size_t h, const char* pointData, const char* edgeData, const char* spaceData)
		: m_width(w), m_height(h), m_pointData(pointData), m_edgeData(edgeData), m_spaceData(spaceData), m_numEvals(0)
	{
		// TODO: assert width/height at least 2, at least one start/end
	}
	
	size_t Puzzle::getNumEvals() const
	{
		return m_numEvals;
	}
	
	size_t Puzzle::getWidth() const
	{
		return m_width;
	}
	
	size_t Puzzle::getHeight() const
	{
		return m_height;
	}
	
	size_t Puzzle::getNumPoints() const
	{
		return getNumPoints(m_width, m_height);
	}
	
	size_t Puzzle::getNumEdges() const
	{
		return getNumEdges(m_width, m_height);
	}
	
	size_t Puzzle::getNumSpaces() const
	{
		return getNumSpaces(m_width, m_height);
	}
	
	size_t Puzzle::getPointIndex(size_t row, size_t col) const
	{
		// TODO: assert row/col in range
		return row*m_width + col;
	}
	
	size_t Puzzle::getPointRow(size_t index) const
	{
		// TODO: assert index in range
		return index/m_width;
	}
	
	size_t Puzzle::getPointCol(size_t index) const
	{
		// TODO: assert index in range
		return index%m_width;
	}
	
	size_t Puzzle::getEdgeIndex(size_t row1, size_t col1, size_t row2, size_t col2) const
	{
		// TODO: assert row/col in range and point1 is above or left of point2
		
		// each row grouping contains w-1 horizontal edges followed by w vertical edges
		size_t index = row1*(m_width*2 - 1) + col1;
		if (col1 == col2) {
			index += m_width - 1;
		}
		
		return index;
	}
	
	size_t Puzzle::getSpaceIndex(size_t row, size_t col) const
	{
		// TODO: assert row/col in range
		return row*(m_width - 1) + col;
	}
	
	size_t Puzzle::getSpaceRow(size_t index) const
	{
		// TODO: assert index in range
		return index/(m_width - 1);
	}
	
	size_t Puzzle::getSpaceCol(size_t index) const
	{
		// TODO: assert index in range
		return index%(m_width - 1);
	}
	
	PointValue Puzzle::getPointValue(size_t row, size_t col) const
	{
		return getPointValue(getPointIndex(row, col));
	}
	
	PointValue Puzzle::getPointValue(size_t index) const
	{
		// TODO: assert index in range
		return (PointValue)m_pointData[index];
	}
	
	EdgeValue Puzzle::getEdgeValue(size_t row1, size_t col1, size_t row2, size_t col2) const
	{
		return getEdgeValue(getEdgeIndex(row1, col1, row2, col2));
	}
	
	EdgeValue Puzzle::getEdgeValue(size_t index) const
	{
		// TODO: assert index in range
		return (EdgeValue)m_edgeData[index];
	}
	
	SpaceValue Puzzle::getSpaceValue(size_t row, size_t col) const
	{
		return getSpaceValue(getSpaceIndex(row, col));
	}
	
	SpaceValue Puzzle::getSpaceValue(size_t index) const
	{
		// TODO: assert index in range
		return (SpaceValue)m_spaceData[index];
	}
	
	bool Puzzle::evaluateSolution(const Path& path) const
	{
		++m_numEvals;
		
		// path must start at a start point
		size_t startIndex = path.getStartPointIndex();
		if (startIndex >= getNumPoints() || m_pointData[path.getStartPointIndex()] != (char)PointValue::START) {
			return false;
		}
		
		// keep track of points visited to make sure no point is visited twice
		bool* visitedPoints = new bool[getNumPoints()];
		for (size_t i = 0; i < getNumPoints(); ++i) {
			visitedPoints[i] = false;
		}
		
		// keep track of edges traversed in order to build partitions
		// to check white/black space constraints later
		bool* visitedEdges = new bool[getNumEdges()];
		for (size_t i = 0; i < getNumEdges(); ++i) {
			visitedEdges[i] = false;
		}
		
		// create list of dots to ensure the path travels through all of them
		bool* pointDots = new bool[getNumPoints()];
		for (size_t i = 0; i < getNumPoints(); ++i) {
			pointDots[i] = (m_pointData[i] == (char)PointValue::DOT);
		}
		
		bool* edgeDots = new bool[getNumEdges()];
		for (size_t i = 0; i < getNumEdges(); ++i) {
			edgeDots[i] = (m_edgeData[i] == (char)EdgeValue::DOT);
		}
		
		// follow path
		bool validPath = true;
		size_t row = getPointRow(startIndex);
		size_t col = getPointCol(startIndex);
		size_t move = 0;
		
		while (validPath && move < path.getNumMoves()) {
			// mark point as visited
			visitedPoints[getPointIndex(row, col)] = true;
			
			// mark off potential point dot
			pointDots[getPointIndex(row, col)] = false;
			
			// validate next move
			switch (path.getMove(move)) {
				case MoveValue::UP:
					validPath = row > 0
					         && getPointValue(row - 1, col) != PointValue::BLOCKED
					         && getEdgeValue(row - 1, col, row, col) != EdgeValue::BLOCKED
					         && !visitedPoints[getPointIndex(row - 1, col)];
					if (validPath) {
						// mark edge as traversed
						visitedEdges[getEdgeIndex(row - 1, col, row, col)] = true;
						
						// mark off potential edge dot
						edgeDots[getEdgeIndex(row - 1, col, row, col)] = false;
						
						// move to next point
						--row;
					}
					break;
				case MoveValue::DOWN:
					validPath = row < getHeight() - 1
					         && getPointValue(row + 1, col) != PointValue::BLOCKED
					         && getEdgeValue(row, col, row + 1, col) != EdgeValue::BLOCKED
					         && !visitedPoints[getPointIndex(row + 1, col)];
					if (validPath) {
						// mark edge as traversed
						visitedEdges[getEdgeIndex(row, col, row + 1, col)] = true;
						
						// mark off potential edge dot
						edgeDots[getEdgeIndex(row, col, row + 1, col)] = false;
						
						// move to next point
						++row;
					}
					break;
				case MoveValue::LEFT:
					validPath = col > 0
					         && getPointValue(row, col - 1) != PointValue::BLOCKED
					         && getEdgeValue(row, col - 1, row, col) != EdgeValue::BLOCKED
					         && !visitedPoints[getPointIndex(row, col - 1)];
					if (validPath) {
						// mark edge as traversed
						visitedEdges[getEdgeIndex(row, col - 1, row, col)] = true;
						
						// mark off potential edge dot
						edgeDots[getEdgeIndex(row, col - 1, row, col)] = false;
						
						// move to next point
						--col;
					}
					break;
				case MoveValue::RIGHT:
					validPath = col < getWidth() - 1
					         && getPointValue(row, col + 1) != PointValue::BLOCKED
					         && getEdgeValue(row, col, row, col + 1) != EdgeValue::BLOCKED
					         && !visitedPoints[getPointIndex(row, col + 1)];
					if (validPath) {
						// mark edge as traversed
						visitedEdges[getEdgeIndex(row, col, row, col + 1)] = true;
						
						// mark off potential edge dot
						edgeDots[getEdgeIndex(row, col, row, col + 1)] = false;
						
						// move to next point
						++col;
					}
					break;
				default:
					break;
			}
			
			++move;
		}
		
		// validate that path ends at an end point
		validPath = validPath && getPointValue(row, col) == PointValue::END;
		
		// validate point and edge dot constraints
		size_t dotIndex = 0;
		while (validPath && dotIndex < getNumPoints()) {
			validPath = !pointDots[dotIndex];
			++dotIndex;
		}
		dotIndex = 0;
		while (validPath && dotIndex < getNumEdges()) {
			validPath = !edgeDots[dotIndex];
			++dotIndex;
		}
		
		// calculate partitions and validate white/black space constraints
		if (validPath) {
			// keep track of which partition each space belongs to
			// and which partitions have white or black spaces
			int* spacePartitionNumbers = new int[getNumSpaces()];
			bool* whitePartitions = new bool[getNumSpaces()];
			bool* blackPartitions = new bool[getNumSpaces()];
			int* searchStack = new int[getNumSpaces()];
			for (size_t i = 0; i < getNumSpaces(); ++i) {
				spacePartitionNumbers[i] = -1;
				whitePartitions[i] = false;
				blackPartitions[i] = false;
				searchStack[i] = -1;
			}
			
			// run depth-first search until solution is invalidated
			// or all spaces are assigned to a partition
			int currentPartition = 0;
			size_t stackSize = 0;
			size_t partitionedSpaces = 0;
			while (validPath && partitionedSpaces < getNumSpaces()) {
				// find starting point for search
				size_t spaceIndex = 0;
				while (spacePartitionNumbers[spaceIndex] >= 0) {
					++spaceIndex;
				}
				
				// index is guaranteed to be valid since when all
				// spaces are assigned this loop won't execute
				searchStack[stackSize] = spaceIndex;
				++stackSize;
				
				while (validPath && stackSize > 0) {
					// pop space from stack
					spaceIndex = searchStack[stackSize - 1];
					--stackSize;
					
					// assign partition number to current space and handle space value
					spacePartitionNumbers[spaceIndex] = currentPartition;
					++partitionedSpaces;
					switch (getSpaceValue(spaceIndex)) {
						case SpaceValue::WHITE:
							whitePartitions[currentPartition] = true;
							break;
						case SpaceValue::BLACK:
							blackPartitions[currentPartition] = true;
							break;
						default:
							break;
					}
					
					// only continue processing if partition constraint hasn't been violated
					if (!(whitePartitions[currentPartition] && blackPartitions[currentPartition])) {
						// check if reachable neighboring spaces need to be processed
						size_t spaceRow = getSpaceRow(spaceIndex);
						size_t spaceCol = getSpaceCol(spaceIndex);
						
						// space row/col coordinates correspond to the same coordinates of the upper-left point on the space
						if (spaceRow > 0 && spacePartitionNumbers[getSpaceIndex(spaceRow - 1, spaceCol)] == -1
						                 && !visitedEdges[getEdgeIndex(spaceRow, spaceCol, spaceRow, spaceCol + 1)]) {
							searchStack[stackSize] = getSpaceIndex(spaceRow - 1, spaceCol);
							++stackSize;
						}
						if (spaceRow < getHeight() - 1 && spacePartitionNumbers[getSpaceIndex(spaceRow + 1, spaceCol)] == -1
						                               && !visitedEdges[getEdgeIndex(spaceRow + 1, spaceCol, spaceRow + 1, spaceCol + 1)]) {
							searchStack[stackSize] = getSpaceIndex(spaceRow + 1, spaceCol);
							++stackSize;
						}
						if (spaceCol > 0 && spacePartitionNumbers[getSpaceIndex(spaceRow, spaceCol - 1)] == -1
						                 && !visitedEdges[getEdgeIndex(spaceRow, spaceCol, spaceRow + 1, spaceCol)]) {
							searchStack[stackSize] = getSpaceIndex(spaceRow, spaceCol - 1);
							++stackSize;
						}
						if (spaceCol < getWidth() - 1 && spacePartitionNumbers[getSpaceIndex(spaceRow, spaceCol + 1)] == -1
						                              && !visitedEdges[getEdgeIndex(spaceRow, spaceCol + 1, spaceRow + 1, spaceCol + 1)]) {
							searchStack[stackSize] = getSpaceIndex(spaceRow, spaceCol + 1);
							++stackSize;
						}
					}
					else {
						validPath = false;
					}
				}
				
				++currentPartition;
			}
			
			delete [] spacePartitionNumbers;
			delete [] whitePartitions;
			delete [] blackPartitions;
			delete [] searchStack;
		}
		
		// clean up memory
		delete [] visitedPoints;
		delete [] visitedEdges;
		delete [] pointDots;
		delete [] edgeDots;
		
		return validPath;
	}
}

std::ostream& operator<<(std::ostream& os, const gws::PointValue& v)
{
	os << (char)v;
	
	return os;
}

std::ostream& operator<<(std::ostream& os, const gws::EdgeValue& v)
{
	os << (char)v;
	
	return os;
}

std::ostream& operator<<(std::ostream& os, const gws::SpaceValue& v)
{
	os << (char)v;
	
	return os;
}