#pragma once
#include "Engine/Math/Vector3.hpp"

#ifndef CAVEPOINT_HPP
#define CAVEPOINT_HPP

struct CavePoint
{
	CavePoint(Vector3 position = Vector3(0.f,0.f,0.f), float width = 0.f);
	const bool operator==(CavePoint other);
	const bool operator!=(CavePoint other);
	Vector3 m_position;
	float m_caveWidthHere;
};

#endif // !CAVEPOINT_HPP
