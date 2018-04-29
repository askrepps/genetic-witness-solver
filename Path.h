//////////////////////////////
// Path.h                   //
// Andrew Krepps            //
// EN.605.417 Final Project //
// 15 May 2018              //
//////////////////////////////

#ifndef gws_Path_h
#define gws_Path_h

#include <ostream>
#include <stddef.h>

namespace gws
{
	///////////////////////////////////////////////////////////////////////////
	/// \enum MoveValue
	/// \brief All possible move direction values
	///////////////////////////////////////////////////////////////////////////
	enum class MoveValue : char
	{
		UP = 'u',
		DOWN = 'd',
		LEFT = 'l',
		RIGHT = 'r',
		NONE = '\0'
	};
	
	///////////////////////////////////////////////////////////////////////////
	/// \class Path
	/// \brief Representation of a path through a puzzle
	///////////////////////////////////////////////////////////////////////////
	class Path
	{
	public:
		///////////////////////////////////////////////////////////////////////
		/// \brief Initialize an empty path (starting point with no moves)
		/// 
		/// The Path object does not assume ownership of the data's memory.
		/// 
		/// \param [in] moveData the buffer for storing move data
		/// \param [in] maxLength the maxiumum length
		///////////////////////////////////////////////////////////////////////
		Path(char* moveData, size_t maxLength);
		
		///////////////////////////////////////////////////////////////////////
		/// \brief Get the starting point index of the path
		/// 
		/// \returns the path's starting point index
		///////////////////////////////////////////////////////////////////////
		size_t getStartPointIndex() const;
		
		///////////////////////////////////////////////////////////////////////
		/// \brief Set the starting point index of the path
		/// 
		/// \param [in] index the path's starting point index
		///////////////////////////////////////////////////////////////////////
		void setStartPointIndex(size_t index);
		
		///////////////////////////////////////////////////////////////////////
		/// \brief Get the max length of the path
		/// 
		/// \returns the path's max length
		///////////////////////////////////////////////////////////////////////
		size_t getMaxLength() const;
		
		///////////////////////////////////////////////////////////////////////
		/// \brief Get the number of moves contained in the path
		/// 
		/// \returns the path's number of moves
		///////////////////////////////////////////////////////////////////////
		size_t getNumMoves() const;
		
		///////////////////////////////////////////////////////////////////////
		/// \brief Get a particular move value in the path
		/// 
		/// \param [in] index the index of the desired move
		/// 
		/// \returns the move value
		///////////////////////////////////////////////////////////////////////
		MoveValue getMove(size_t index) const;
		
		///////////////////////////////////////////////////////////////////////
		/// \brief Append a move to the end of the path
		/// 
		/// If the path has already reached its max length, the request will
		/// be ignored.
		/// 
		/// \param [in] value the move
		///////////////////////////////////////////////////////////////////////
		void addMove(const MoveValue& value);
		
		///////////////////////////////////////////////////////////////////////
		/// \brief Remove the last move from the end of the path
		/// 
		/// If the path is empty, the request will be ignored.
		///////////////////////////////////////////////////////////////////////
		void popMove();
		
	private:
		char* m_moveData;
		size_t m_maxLength;
		size_t m_numMoves;
		
		size_t m_startPointIndex;
	};
}

///////////////////////////////////////////////////////////////////////////////
/// \brief Send a move value to an output stream
/// 
/// \param [in] os the output stream
/// \param [in] v the move value
/// 
/// \returns the output stream
///////////////////////////////////////////////////////////////////////////////
std::ostream& operator<<(std::ostream& os, const gws::MoveValue& v);

///////////////////////////////////////////////////////////////////////////////
/// \brief Send a path to an output stream
/// 
/// \param [in] os the output stream
/// \param [in] p the path
/// 
/// \returns the output stream
///////////////////////////////////////////////////////////////////////////////
std::ostream& operator<<(std::ostream& os, const gws::Path& p);

#endif