//////////////////////////////
// Path.cpp                 //
// Andrew Krepps            //
// EN.605.417 Final Project //
// 15 May 2018              //
//////////////////////////////

#include "Path.h"

#include <stddef.h>

namespace gws
{
	Path::Path(size_t startPointIndex, char* moveData, size_t maxLength)
		: m_startPointIndex(startPointIndex), m_moveData(moveData), m_maxLength(maxLength), m_numMoves(0)
	{
		// make sure move data buffer is initialized
		for (size_t i = 0; i < m_maxLength; ++i) {
			m_moveData[i] = (char)MoveValue::NONE;
		}
	}
	
	size_t Path::getStartPointIndex() const
	{
		return m_startPointIndex;
	}
	
	size_t Path::getMaxLength() const
	{
		return m_maxLength;
	}
	
	size_t Path::getNumMoves() const
	{
		return m_numMoves;
	}
	
	void Path::addMove(const MoveValue& value)
	{
		if (m_numMoves < m_maxLength) {
			m_moveData[m_numMoves++] = (char)value;
		}
	}
	
	void Path::popMove()
	{
		if (m_numMoves > 0) {
			m_moveData[m_numMoves--] = (char)MoveValue::NONE;
		}
	}
}