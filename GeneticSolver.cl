//////////////////////////////
// GeneticSolver.cl         //
// Andrew Krepps            //
// EN.605.417 Final Project //
// 15 May 2018              //
//////////////////////////////

#define POINT_OPEN 'o'
#define POINT_BLOCKED 'x'
#define POINT_DOT '.'
#define POINT_START 's'
#define POINT_END 'e'

#define EDGE_OPEN 'o'
#define EDGE_BLOCKED 'x'
#define EDGE_DOT '.'

#define SPACE_BLANK '_'
#define SPACE_WHITE 'w'
#define SPACE_BLACK 'b'

#define NUM_POSSIBLE_MOVES 4
#define MOVE_UP 'u'
#define MOVE_DOWN 'd'
#define MOVE_LEFT 'l'
#define MOVE_RIGHT 'r'
#define MOVE_NONE '\0'

unsigned int getPointIndex(
	const unsigned int row,
	const unsigned int col,
	const unsigned int width)
{
	return row*width + col;
}

unsigned int getEdgeIndex(
	const unsigned int row1,
	const unsigned int col1,
	const unsigned int row2,
	const unsigned int col2,
	const unsigned int width)
{
	// each row grouping contains w-1 horizontal edges
	// followed by w vertical edges
	unsigned int index = row1*(width*2 - 1) + col1;
	if (col1 == col2) {
		index += width - 1;
	}
	
	return index;
}

unsigned int getSpaceIndex(
	const unsigned int row,
	const unsigned int col,
	const unsigned int width)
{
	return row*(width - 1) + col;
}

unsigned int getPointRow(
	const unsigned int index,
	const unsigned int width)
{
	return index/width;
}

unsigned int getPointCol(
	const unsigned int index,
	const unsigned int width)
{
	return index%width;
}

unsigned int getSpaceRow(
	const unsigned int index,
	const unsigned int width)
{
	return index/(width - 1);
}

unsigned int getSpaceCol(
	const unsigned int index,
	const unsigned int width)
{
	return index%(width - 1);
}

unsigned int findStartIndex(
	const unsigned char initialValue,
	__constant const char* puzzlePoints,
	const unsigned int numPoints)
{
	unsigned char value = initialValue;
	unsigned int index = 0;
	while (value > 0) {
		if (puzzlePoints[index] == POINT_START) {
			--value;
		}
		
		if (value > 0) {
			index = (index + 1)%numPoints;
		}
	}
	
	return index;
}

unsigned int getNextEdge(
	const unsigned int currentRow,
	const unsigned int currentCol,
	const unsigned int nextRow,
	const unsigned int nextCol,
	const unsigned int puzzleWidth)
{
	// traversed edge is either right/down or left/up from current point
	// this assumes current and next coordinates are valid
	unsigned int nextEdge;
	if (currentRow < nextRow || currentCol < nextCol) {
		nextEdge = getEdgeIndex(currentRow, currentCol, nextRow, nextCol, puzzleWidth);
	}
	else {
		nextEdge = getEdgeIndex(nextRow, nextCol, currentRow, currentCol, puzzleWidth);
	}
	
	return nextEdge;
}

bool checkMove(
	const unsigned int currentRow,
	const unsigned int currentCol,
	const unsigned int nextRow,
	const unsigned int nextCol,
	__constant const char* puzzlePoints,
	__constant const char* puzzleEdges,
	const unsigned int numPoints,
	const unsigned int puzzleWidth,
	const unsigned int puzzleHeight,
	__local bool* visitedPoints,
	const unsigned int localPointStartIndex)
{
	// get desired point index
	unsigned int nextPoint = getPointIndex(nextRow, nextCol, puzzleWidth);
	unsigned int nextEdge = getNextEdge(currentRow, currentCol, nextRow, nextCol, puzzleWidth);
	
	// move is valid if point is in range, the point and edge aren't blocked,
	// and we haven't been there before
	return nextPoint < numPoints
	    && nextRow < puzzleHeight
	    && nextCol < puzzleWidth
	    && puzzlePoints[nextPoint] != POINT_BLOCKED
	    && puzzleEdges[nextEdge] != EDGE_BLOCKED
	    && !visitedPoints[localPointStartIndex + nextPoint];
}

bool checkMoveUp(
	const unsigned int currentRow,
	const unsigned int currentCol,
	__constant const char* puzzlePoints,
	__constant const char* puzzleEdges,
	const unsigned int numPoints,
	const unsigned int puzzleWidth,
	const unsigned int puzzleHeight,
	__local bool* visitedPoints,
	const unsigned int localPointStartIndex)
{
	return checkMove(
		currentRow,
		currentCol,
		currentRow - 1,
		currentCol,
		puzzlePoints,
		puzzleEdges,
		numPoints,
		puzzleWidth,
		puzzleHeight,
		visitedPoints,
		localPointStartIndex);
}

bool checkMoveDown(
	const unsigned int currentRow,
	const unsigned int currentCol,
	__constant const char* puzzlePoints,
	__constant const char* puzzleEdges,
	const unsigned int numPoints,
	const unsigned int puzzleWidth,
	const unsigned int puzzleHeight,
	__local bool* visitedPoints,
	const unsigned int localPointStartIndex)
{
	return checkMove(
		currentRow,
		currentCol,
		currentRow + 1,
		currentCol,
		puzzlePoints,
		puzzleEdges,
		numPoints,
		puzzleWidth,
		puzzleHeight,
		visitedPoints,
		localPointStartIndex);
}

bool checkMoveLeft(
	const unsigned int currentRow,
	const unsigned int currentCol,
	__constant const char* puzzlePoints,
	__constant const char* puzzleEdges,
	const unsigned int numPoints,
	const unsigned int puzzleWidth,
	const unsigned int puzzleHeight,
	__local bool* visitedPoints,
	const unsigned int localPointStartIndex)
{
	return checkMove(
		currentRow,
		currentCol,
		currentRow,
		currentCol - 1,
		puzzlePoints,
		puzzleEdges,
		numPoints,
		puzzleWidth,
		puzzleHeight,
		visitedPoints,
		localPointStartIndex);
}

bool checkMoveRight(
	const unsigned int currentRow,
	const unsigned int currentCol,
	__constant const char* puzzlePoints,
	__constant const char* puzzleEdges,
	const unsigned int numPoints,
	const unsigned int puzzleWidth,
	const unsigned int puzzleHeight,
	__local bool* visitedPoints,
	const unsigned int localPointStartIndex)
{
	return checkMove(
		currentRow,
		currentCol,
		currentRow,
		currentCol + 1,
		puzzlePoints,
		puzzleEdges,
		numPoints,
		puzzleWidth,
		puzzleHeight,
		visitedPoints,
		localPointStartIndex);
}

__kernel void evaluatePopulation(
	__global const unsigned char* population,
	const unsigned int popSize,
	const unsigned int maxLength,
	__constant const char* puzzlePoints,
	__constant const char* puzzleEdges,
	__constant const char* puzzleSpaces,
	const unsigned int numPoints,
	const unsigned int numEdges,
	const unsigned int numSpaces,
	const unsigned int puzzleWidth,
	const unsigned int puzzleHeight,
	__local bool* visitedPoints,
	__local bool* visitedEdges,
	__local char* spacePartitionNumbers,
	__local bool* whitePartitions,
	__local bool* blackPartitions,
	__local unsigned char* searchStack,
	__global int* fitness,
	__global unsigned int* startPoints,
	__global char* paths)
{
	unsigned int gid = get_global_id(0);
	if (gid < popSize) {
		int fitnessValue = 0;
		
		// calculate first index of memory arrays used by this work item
		unsigned int popStartIndex = gid*maxLength;
		
		unsigned int lid = get_local_id(0);
		unsigned int localPointStartIndex = lid*numPoints;
		unsigned int localEdgeStartIndex = lid*numEdges;
		unsigned int localSpaceStartIndex = lid*numSpaces;
		
		// mark all points and edges as unvisited (they can only be visited once)
		for (unsigned int i = 0; i < numPoints; ++i) {
			visitedPoints[localPointStartIndex + i] = false;
		}
		for (unsigned int i = 0; i < numEdges; ++i) {
			visitedEdges[localEdgeStartIndex + i] = false;
		}
		
		// initialize paths to empty
		for (unsigned int i = 0; i < numPoints; ++i) {
			paths[popStartIndex + i] = MOVE_NONE;
		}
		
		// find start point of path
		bool canContinue = true;
		unsigned int startIndex = findStartIndex(population[popStartIndex], puzzlePoints, numPoints);
		unsigned int row = getPointRow(startIndex, puzzleWidth);
		unsigned int col = getPointCol(startIndex, puzzleWidth);
		unsigned int nextRow = row;
		unsigned int nextCol = col;
		unsigned int move = 0;
		
		// output start point of path
		startPoints[gid] = startIndex;
		
		// follow path
		while (canContinue && move < maxLength) {
			unsigned int currentPointIndex = getPointIndex(row, col, puzzleWidth);
			
			// mark point as visited
			visitedPoints[localPointStartIndex + currentPointIndex] = true;
			
			// check dot constraint
			if (puzzlePoints[currentPointIndex] == POINT_DOT) {
				// dots earn 1 fitness point
				++fitnessValue;
			}
			
			// only continue  reached the end of the puzzle
			if (puzzlePoints[currentPointIndex] != POINT_END) {
				// get next move (check which moves are valid)
				char possibleMoves[NUM_POSSIBLE_MOVES];
				unsigned int choices = 0;
				if (checkMoveUp(row, col, puzzlePoints, puzzleEdges, numPoints,
				                puzzleWidth, puzzleHeight, visitedPoints, localPointStartIndex)) {
					possibleMoves[choices] = MOVE_UP;
					++choices;
				}
				if (checkMoveDown(row, col, puzzlePoints, puzzleEdges, numPoints,
				                  puzzleWidth, puzzleHeight, visitedPoints, localPointStartIndex)) {
					possibleMoves[choices] = MOVE_DOWN;
					++choices;
				}
				if (checkMoveLeft(row, col, puzzlePoints, puzzleEdges, numPoints,
				                  puzzleWidth, puzzleHeight, visitedPoints, localPointStartIndex)) {
					possibleMoves[choices] = MOVE_LEFT;
					++choices;
				}
				if (checkMoveRight(row, col, puzzlePoints, puzzleEdges, numPoints,
				                   puzzleWidth, puzzleHeight, visitedPoints, localPointStartIndex)) {
					possibleMoves[choices] = MOVE_RIGHT;
					++choices;
				}
				
				// if moves are possible continue, otherwise
				if (choices > 0) {
					nextRow = row;
					nextCol = col;
					switch (possibleMoves[population[popStartIndex + move] % choices]) {
						case MOVE_UP:
							nextRow = row - 1;
							paths[popStartIndex + move] = MOVE_UP;
							break;
						case MOVE_DOWN:
							nextRow = row + 1;
							paths[popStartIndex + move] = MOVE_DOWN;
							break;
						case MOVE_LEFT:
							nextCol = col - 1;
							paths[popStartIndex + move] = MOVE_LEFT;
							break;
						case MOVE_RIGHT:
							nextCol = col + 1;
							paths[popStartIndex + move] = MOVE_RIGHT;
							break;
						default:
							// something weird happened, so stop
							canContinue = false;
							break;
					}
					
					// mark edge as visited
					unsigned int nextEdge = getNextEdge(row, col, nextRow, nextCol, puzzleWidth);
					visitedEdges[localEdgeStartIndex + nextEdge] = true;
					
					// check dot constraint
					if (puzzleEdges[nextEdge] == EDGE_DOT) {
						// dots earn 1 fitness point
						++fitnessValue;
					}
					
					row = nextRow;
					col = nextCol;
				}
				else {
					canContinue = false;
				}	
			}
			else {
				// reaching the end earns 1 fitness point
				++fitnessValue;
				canContinue = false;
			}
			
			++move;
		}
		
		// validate white/black space parition constraints
		
		// keep track of which partition each space belongs to
		// and which partitions have white or black spaces
		for (size_t i = 0; i < numSpaces; ++i) {
			spacePartitionNumbers[localSpaceStartIndex + i] = -1;
			whitePartitions[localSpaceStartIndex + i] = false;
			blackPartitions[localSpaceStartIndex + i] = false;
			searchStack[localSpaceStartIndex + i] = false;
		}
		
		// run depth-first search until all spaces are partitioned
		char currentPartition = 0;
		unsigned int stackSize = 0;
		unsigned int partitionedSpaces = 0;
		while (partitionedSpaces < numSpaces) {
			// find starting point for search
			unsigned int spaceIndex = 0;
			while (spacePartitionNumbers[localSpaceStartIndex + spaceIndex] >= 0) {
				++spaceIndex;
			}
			
			// index is guaranteed to be valid since when all
			// spaces are assigned this loop won't execute
			searchStack[localSpaceStartIndex + stackSize] = spaceIndex;
			++stackSize;
			
			while (stackSize > 0) {
				// pop space from stack
				spaceIndex = searchStack[localSpaceStartIndex + stackSize - 1];
				--stackSize;
				
				// assign partition number to current space and handle space value
				spacePartitionNumbers[localSpaceStartIndex + spaceIndex] = currentPartition;
				++partitionedSpaces;
				switch (puzzleSpaces[spaceIndex]) {
					case SPACE_WHITE:
						whitePartitions[localSpaceStartIndex + currentPartition] = true;
						break;
					case SPACE_BLACK:
						blackPartitions[localSpaceStartIndex + currentPartition] = true;
						break;
					default:
						break;
				}
				
				// check if reachable neighboring spaces need to be processed
				unsigned int spaceRow = getSpaceRow(spaceIndex, puzzleWidth);
				unsigned int spaceCol = getSpaceCol(spaceIndex, puzzleWidth);
				
				// space row/col coordinates correspond to the same coordinates of the upper-left point on the space
				if (spaceRow > 0 && spacePartitionNumbers[localSpaceStartIndex + getSpaceIndex(spaceRow - 1, spaceCol, puzzleWidth)] == -1
				                 && !visitedEdges[localEdgeStartIndex + getEdgeIndex(spaceRow, spaceCol, spaceRow, spaceCol + 1, puzzleWidth)]) {
					searchStack[localSpaceStartIndex + stackSize] = getSpaceIndex(spaceRow - 1, spaceCol, puzzleWidth);
					++stackSize;
				}
				if (spaceRow < puzzleHeight - 2 && spacePartitionNumbers[localSpaceStartIndex + getSpaceIndex(spaceRow + 1, spaceCol, puzzleWidth)] == -1
				                               && !visitedEdges[localEdgeStartIndex + getEdgeIndex(spaceRow + 1, spaceCol, spaceRow + 1, spaceCol + 1, puzzleWidth)]) {
					searchStack[localSpaceStartIndex + stackSize] = getSpaceIndex(spaceRow + 1, spaceCol, puzzleWidth);
					++stackSize;
				}
				if (spaceCol > 0 && spacePartitionNumbers[localSpaceStartIndex + getSpaceIndex(spaceRow, spaceCol - 1, puzzleWidth)] == -1
				                 && !visitedEdges[localEdgeStartIndex + getEdgeIndex(spaceRow, spaceCol, spaceRow + 1, spaceCol, puzzleWidth)]) {
					searchStack[localSpaceStartIndex + stackSize] = getSpaceIndex(spaceRow, spaceCol - 1, puzzleWidth);
					++stackSize;
				}
				if (spaceCol < puzzleWidth - 2 && spacePartitionNumbers[localSpaceStartIndex + getSpaceIndex(spaceRow, spaceCol + 1, puzzleWidth)] == -1
				                              && !visitedEdges[localEdgeStartIndex + getEdgeIndex(spaceRow, spaceCol + 1, spaceRow + 1, spaceCol + 1, puzzleWidth)]) {
					searchStack[localSpaceStartIndex + stackSize] = getSpaceIndex(spaceRow, spaceCol + 1, puzzleWidth);
					++stackSize;
				}
			}
			
			++currentPartition;
		}
		
		// each white/black space in a valid partition earns 1 fitness point
		for (size_t i = 0; i < numSpaces; ++i) {
			char partition = spacePartitionNumbers[localSpaceStartIndex + i];
			switch (puzzleSpaces[i]) {
				case SPACE_WHITE:
					if (whitePartitions[localSpaceStartIndex + partition] && !blackPartitions[localSpaceStartIndex + partition]) {
						++fitnessValue;
					}
					break;
				case SPACE_BLACK:
					if (blackPartitions[localSpaceStartIndex + partition] && !whitePartitions[localSpaceStartIndex + partition]) {
						++fitnessValue;
					}
					break;
				default:
					break;
			}
		}
		
		fitness[gid] = fitnessValue;
	}
}