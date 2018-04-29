//////////////////////////////
// Puzzle.h                 //
// Andrew Krepps            //
// EN.605.417 Final Project //
// 15 May 2018              //
//////////////////////////////

#ifndef gws_Puzzle_h
#define gws_Puzzle_h

#include <ostream>
#include <stddef.h>

namespace gws
{
	///////////////////////////////////////////////////////////////////////////
	/// \enum PointValue
	/// \brief All possible puzzle intersection point values
	///////////////////////////////////////////////////////////////////////////
	enum class PointValue : char
	{
		OPEN = 'o',
		BLOCKED = 'x',
		DOT = '.',
		START = 's',
		END = 'e'
	};
	
	///////////////////////////////////////////////////////////////////////////
	/// \enum EdgeValue
	/// \brief All possible puzzle edge values
	///////////////////////////////////////////////////////////////////////////
	enum class EdgeValue : char
	{
		OPEN = 'o',
		BLOCKED = 'x',
		DOT = '.'
	};
	
	///////////////////////////////////////////////////////////////////////////
	/// \enum SpaceValue
	/// \brief All possible puzzle space values
	///////////////////////////////////////////////////////////////////////////
	enum class SpaceValue : char
	{
		BLANK = '_',
		WHITE = 'w',
		BLACK = 'b'
	};
	
	///////////////////////////////////////////////////////////////////////////
	/// \class Puzzle
	/// \brief Representation of a line puzzle from The Witness
	/// 
	/// A Puzzle consists of intersection points on a rectangular grid, edges
	/// connecting each point, and spaces inside the boxes formed by edges.
	/// 
	/// All puzzle element data is specified starting at the top left corner,
	/// moving from left to right and top to bottom. Edge data alternates
	/// between horizontal edges and vertical edges
	///////////////////////////////////////////////////////////////////////////
	class Puzzle
	{
	public:
		///////////////////////////////////////////////////////////////////////
		/// \brief Get the number of intersection points a puzzle with given
		/// dimensions would have
		/// 
		/// \param [in] width the width of the puzzle
		/// \param [in] height the height of the puzzle
		/// 
		/// \returns the number of points
		///////////////////////////////////////////////////////////////////////
		static size_t getNumPoints(size_t width, size_t height);
		
		///////////////////////////////////////////////////////////////////////
		/// \brief Get the number of edges a puzzle with given dimensions
		/// would have
		/// 
		/// \param [in] width the width of the puzzle
		/// \param [in] height the height of the puzzle
		/// 
		/// \returns the number of edges
		///////////////////////////////////////////////////////////////////////
		static size_t getNumEdges(size_t width, size_t height);
		
		///////////////////////////////////////////////////////////////////////
		/// \brief Get the number of spaces a puzzle with given dimensions
		/// would have
		/// 
		/// \param [in] width the width of the puzzle
		/// \param [in] height the height of the puzzle
		/// 
		/// \returns the number of spaces
		///////////////////////////////////////////////////////////////////////
		static size_t getNumSpaces(size_t width, size_t height);
		
		///////////////////////////////////////////////////////////////////////
		/// \brief Initialize a puzzle from existing data
		/// 
		/// The Puzzle object does not assume ownership of the data's memory.
		/// 
		/// \param [in] w the number of intersection points in each row
		/// \param [in] h the number of intersection points in each column
		/// \param [in] pointData each intersection point [w*h elements]
		/// \param [in] edgeData each edge [(w-1)*h + w*(h-1) elements]
		/// \param [in] spaceData each space [(w-1)*(h-1) elements]
		///////////////////////////////////////////////////////////////////////
		Puzzle(size_t w, size_t h, const char* pointData, const char* edgeData, const char* spaceData);
		
		///////////////////////////////////////////////////////////////////////
		/// \brief Get the width of the puzzle
		/// 
		/// \returns the number of points per row
		///////////////////////////////////////////////////////////////////////
		size_t getWidth() const;
		
		///////////////////////////////////////////////////////////////////////
		/// \brief Get the height of the puzzle
		/// 
		/// \returns the number of points per column
		///////////////////////////////////////////////////////////////////////
		size_t getHeight() const;
		
		///////////////////////////////////////////////////////////////////////
		/// \brief Get the number of intersection points in the puzzle
		/// 
		/// \returns the number of points
		///////////////////////////////////////////////////////////////////////
		size_t getNumPoints() const;
		
		///////////////////////////////////////////////////////////////////////
		/// \brief Get the number of edges in the puzzle
		/// 
		/// \returns the number of edges
		///////////////////////////////////////////////////////////////////////
		size_t getNumEdges() const;
		
		///////////////////////////////////////////////////////////////////////
		/// \brief Get the number of spaces in the puzzle
		/// 
		/// \returns the number of spaces
		///////////////////////////////////////////////////////////////////////
		size_t getNumSpaces() const;
		
		///////////////////////////////////////////////////////////////////////
		/// \brief Get the index of an intersection point
		/// 
		/// \param [in] row the point row (in range [0..h-1])
		/// \param [in] col the point column (in range [0..w-1])
		/// 
		/// \returns the index of the given point
		///////////////////////////////////////////////////////////////////////
		size_t getPointIndex(size_t row, size_t col) const;
		
		///////////////////////////////////////////////////////////////////////
		/// \brief Get the row number of a point
		/// 
		/// \param [in] index the index of the point
		/// 
		/// \returns the row number of the point
		///////////////////////////////////////////////////////////////////////
		size_t getPointRow(size_t index) const;
		
		///////////////////////////////////////////////////////////////////////
		/// \brief Get the column number of a point
		/// 
		/// \param [in] index the index of the point
		/// 
		/// \returns the column number of the point
		///////////////////////////////////////////////////////////////////////
		size_t getPointCol(size_t index) const;
		
		///////////////////////////////////////////////////////////////////////
		/// \brief Get the index of an edge between two points
		/// 
		/// \param [in] row1 the first point row (in range [0..h-1])
		/// \param [in] col1 the first point column (in range [0..w-1])
		/// \param [in] row2 the second point row (in range [0..h-1])
		/// \param [in] col2 the second point column (in range [0..w-1])
		/// 
		/// \returns the index of the given edge
		///////////////////////////////////////////////////////////////////////
		size_t getEdgeIndex(size_t row1, size_t col1, size_t row2, size_t col2) const;
		
		///////////////////////////////////////////////////////////////////////
		/// \brief Get the index of a space
		/// 
		/// \param [in] row the space row (in range [0..h-2])
		/// \param [in] col the space column (in range [0..w-2])
		/// 
		/// \returns the index of the given space
		///////////////////////////////////////////////////////////////////////
		size_t getSpaceIndex(size_t row, size_t col) const;
		
		///////////////////////////////////////////////////////////////////////
		/// \brief Get the row number of a space
		/// 
		/// \param [in] index the index of the space
		/// 
		/// \returns the row number of the space
		///////////////////////////////////////////////////////////////////////
		size_t getSpaceRow(size_t index) const;
		
		///////////////////////////////////////////////////////////////////////
		/// \brief Get the column number of a space
		/// 
		/// \param [in] index the index of the space
		/// 
		/// \returns the column number of the space
		///////////////////////////////////////////////////////////////////////
		size_t getSpaceCol(size_t index) const;
		
		///////////////////////////////////////////////////////////////////////
		/// \brief Get the value of an intersection point
		/// 
		/// \param [in] row the point row (in range [0..h-1])
		/// \param [in] col the point column (in range [0..w-1])
		/// 
		/// \returns the value of the given point
		///////////////////////////////////////////////////////////////////////
		PointValue getPointValue(size_t row, size_t col) const;
		
		///////////////////////////////////////////////////////////////////////
		/// \brief Get the value of an edge between two points
		/// 
		/// \param [in] row1 the first point row (in range [0..h-1])
		/// \param [in] col1 the first point column (in range [0..w-1])
		/// \param [in] row2 the second point row (in range [0..h-1])
		/// \param [in] col2 the second point column (in range [0..w-1])
		/// 
		/// \returns the value of the given edge
		///////////////////////////////////////////////////////////////////////
		EdgeValue getEdgeValue(size_t row1, size_t col1, size_t row2, size_t col2) const;
		
		///////////////////////////////////////////////////////////////////////
		/// \brief Get the value of a space
		/// 
		/// \param [in] row the space row (in range [0..h-2])
		/// \param [in] col the space column (in range [0..w-2])
		/// 
		/// \returns the value of the given space
		///////////////////////////////////////////////////////////////////////
		SpaceValue getSpaceValue(size_t row, size_t col) const;
		
	private:
		size_t m_width;
		size_t m_height;
		
		const char* m_pointData;
		const char* m_edgeData;
		const char* m_spaceData;
	};
}

///////////////////////////////////////////////////////////////////////////////
/// \brief Send a point value to an output stream
/// 
/// \param [in] os the output stream
/// \param [in] v the point value
/// 
/// \returns the output stream
///////////////////////////////////////////////////////////////////////////////
std::ostream& operator<<(std::ostream& os, const gws::PointValue& v);

///////////////////////////////////////////////////////////////////////////////
/// \brief Send an edge value to an output stream
/// 
/// \param [in] os the output stream
/// \param [in] v the edge value
/// 
/// \returns the output stream
///////////////////////////////////////////////////////////////////////////////
std::ostream& operator<<(std::ostream& os, const gws::EdgeValue& v);

///////////////////////////////////////////////////////////////////////////////
/// \brief Send a space value to an output stream
/// 
/// \param [in] os the output stream
/// \param [in] v the space value
/// 
/// \returns the output stream
///////////////////////////////////////////////////////////////////////////////
std::ostream& operator<<(std::ostream& os, const gws::SpaceValue& v);

#endif