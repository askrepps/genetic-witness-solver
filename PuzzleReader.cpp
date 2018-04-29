//////////////////////////////
// PuzzleReader.cpp         //
// Andrew Krepps            //
// EN.605.417 Final Project //
// 15 May 2018              //
//////////////////////////////

#include "PuzzleReader.h"

#include "Puzzle.h"

#include <fstream>
#include <stddef.h>
#include <stdexcept>
#include <sstream>
#include <string>

#include <iostream>

namespace gws
{
	void readPuzzleData(const std::string& puzzleFile, char** pointBuffer, char** edgeBuffer, char** spaceBuffer, size_t* width, size_t* height)
	{
		// open the file
		std::ifstream fileStream(puzzleFile);
		
		if (fileStream.is_open()) {
			// read puzzle dimensions
			fileStream >> *width;
			fileStream >> *height;
			
			// allocate memory for buffers
			*pointBuffer = new char[Puzzle::getNumPoints(*width, *height)];
			*edgeBuffer = new char[Puzzle::getNumEdges(*width, *height)];
			*spaceBuffer = new char[Puzzle::getNumSpaces(*width, *height)];
			
			// read puzzle data
			size_t pointIndex = 0;
			size_t edgeIndex = 0;
			size_t spaceIndex = 0;
			for (size_t row = 0; row < *height; ++row) {
				// read row of points/edges
				for (size_t col = 0; col < *width; ++col) {
					fileStream >> (*pointBuffer)[pointIndex++];
					if (col < *width - 1) {
						fileStream >> (*edgeBuffer)[edgeIndex++];
					}
				}
				
				// read row of edges/spaces
				if (row < *height - 1) {
					for (size_t col = 0; col < *width; ++col) {
						fileStream >> (*edgeBuffer)[edgeIndex++];
						if (col < *width - 1) {
							fileStream >> (*spaceBuffer)[spaceIndex++];
						}
					}
				}
			}
			
			// close the file
			fileStream.close();
		}
		else {
			std::ostringstream oss;
			oss << "Could not open file '" << puzzleFile << "'";
			
			throw std::runtime_error(oss.str());
		}
	}
	
	Puzzle readPuzzle(const std::string& puzzleFile, char** pointBuffer, char** edgeBuffer, char** spaceBuffer)
	{
		size_t width;
		size_t height;
		
		readPuzzleData(puzzleFile, pointBuffer, edgeBuffer, spaceBuffer, &width, &height);
		
		return Puzzle(width, height, *pointBuffer, *edgeBuffer, *spaceBuffer);
	}
}