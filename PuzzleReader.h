//////////////////////////////
// PuzzleReader.h           //
// Andrew Krepps            //
// EN.605.417 Final Project //
// 15 May 2018              //
//////////////////////////////

#ifndef gws_PuzzleReader_h
#define gws_PuzzleReader_h

#include "Puzzle.h"

#include <stddef.h>
#include <string>

namespace gws
{
	///////////////////////////////////////////////////////////////////////////
	/// \brief Read puzzle data from a file
	/// 
	/// This function will allocate memory for the point, edge, and space
	/// buffers, and it is the caller's responsibility to free this memory
	/// later using delete [].
	/// 
	/// \param [in] puzzleFile the path to the puzzle file
	/// \param [out] pointBuffer the buffer containing the point data
	/// \param [out] edgeBuffer the buffer containing the edge data
	/// \param [out] spaceBuffer the buffer containing the space data
	/// \param [out] width the puzzle width
	/// \param [out] height the puzzle height
	/// 
	/// \throws std::runtime_error if file reading was unsuccessful
	///////////////////////////////////////////////////////////////////////////
	void readPuzzleData(const std::string& puzzleFile, char** pointBuffer, char** edgeBuffer, char** spaceBuffer, size_t* width, size_t* height);
	
	///////////////////////////////////////////////////////////////////////////
	/// \brief Read puzzle data from a file and store it in a puzzle object
	/// 
	/// This function will allocate memory for the point, edge, and space
	/// buffers, and it is the caller's responsibility to free this memory
	/// later using delete [].
	/// 
	/// \param [in] puzzleFile the path to the puzzle file
	/// \param [out] pointBuffer the buffer containing the point data
	/// \param [out] edgeBuffer the buffer containing the edge data
	/// \param [out] spaceBuffer the buffer containing the space data
	/// 
	/// \returns the puzzle object
	/// 
	/// \throws std::runtime_error if file reading was unsuccessful
	///////////////////////////////////////////////////////////////////////////
	Puzzle readPuzzle(const std::string& puzzleFile, char** pointBuffer, char** edgeBuffer, char** spaceBuffer);
}

#endif