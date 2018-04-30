//////////////////////////////
// Path.cpp                 //
// Andrew Krepps            //
// EN.605.417 Final Project //
// 15 May 2018              //
//////////////////////////////

#include "Path.h"

#include <ostream>
#include <stddef.h>

namespace gws
{
	Path::Path(char* moveData, size_t maxLength)
		: m_moveData(moveData), m_maxLength(maxLength), m_numMoves(0), m_startPointIndex(0)
	{
		// make sure move data buffer is initialized
		clear();
	}
	
	size_t Path::getStartPointIndex() const
	{
		return m_startPointIndex;
	}
	
	void Path::setStartPointIndex(size_t index)
	{
		m_startPointIndex = index;
	}
	
	size_t Path::getMaxLength() const
	{
		return m_maxLength;
	}
	
	size_t Path::getNumMoves() const
	{
		return m_numMoves;
	}
	
	MoveValue Path::getMove(size_t index) const
	{
		// TODO: assert index in range
		return (MoveValue)m_moveData[index];
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
	
	void Path::clear()
	{
		for (size_t i = 0; i < m_maxLength; ++i) {
			m_moveData[i] = (char)MoveValue::NONE;
		}
		m_numMoves = 0;
		m_startPointIndex = 0;
	}
}

std::ostream& operator<<(std::ostream& os, const gws::MoveValue& v)
{
	os << (char)v;
	
	return os;
}

std::ostream& operator<<(std::ostream& os, const gws::Path& p)
{
	for (size_t i = 0; i < p.getNumMoves(); ++i) {
		os << p.getMove(i);
	}
	
	return os;
}