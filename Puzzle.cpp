//////////////////////////////
// Puzzle.cpp               //
// Andrew Krepps            //
// EN.605.417 Final Project //
// 15 May 2018              //
//////////////////////////////

#include "Puzzle.h"

namespace gws
{
	Puzzle::Puzzle(size_t w, size_t h, const char* pointData, const char* edgeData, const char* spaceData)
		: m_width(w), m_height(h), m_pointData(pointData), m_edgeData(edgeData), m_spaceData(spaceData)
	{
		// TODO: assert width/height at least 2, at least one start/end
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
		return m_width*m_height;
	}
	
	size_t Puzzle::getNumEdges() const
	{
		// horizontal edges + vertical edges
		return (m_width - 1)*m_height + m_width*(m_height - 1);
	}
	
	size_t Puzzle::getNumSpaces() const
	{
		return (m_width - 1)*(m_height - 1);
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
		return (PointValue)m_pointData[getPointIndex(row, col)];
	}
	
	EdgeValue Puzzle::getEdgeValue(size_t row1, size_t col1, size_t row2, size_t col2) const
	{
		return (EdgeValue)m_edgeData[getEdgeIndex(row1, col1, row2, col2)];
	}
	
	SpaceValue Puzzle::getSpaceValue(size_t row, size_t col) const
	{
		return (SpaceValue)m_spaceData[getSpaceIndex(row, col)];
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