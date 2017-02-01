#include "Game/CavePoint.hpp"

CavePoint::CavePoint(Vector3 position, float width)
: m_position(position),
m_caveWidthHere(width)
{
}

const bool CavePoint::operator==(CavePoint other)
{
	if (other.m_position == m_position
		&& other.m_caveWidthHere == m_caveWidthHere)
	{
		return true;
	}
	return false;
}

const bool CavePoint::operator!=(CavePoint other)
{
	if (other.m_position != m_position
		|| other.m_caveWidthHere != m_caveWidthHere)
	{
		return true;
	}
	return false;
}