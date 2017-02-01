#include "Engine/Math/IntVector2.hpp"
#include <Math.h>

IntVector2::IntVector2(const int& newX, const int& newY)
:x(newX), y(newY)
{
}
IntVector2::IntVector2(const IntVector2& other)
: x(other.x), y(other.y)
{
}

const float IntVector2::getLength() const
{
	return sqrt( (float)((x*x) + (y*y)));
}
const IntVector2 IntVector2::operator+(const IntVector2& other) const
{
	return IntVector2(x + other.x, y + other.y);
}
const IntVector2 IntVector2::operator-(const IntVector2& other) const
{
	return IntVector2(x - other.x, y - other.y);
}
const IntVector2 IntVector2::operator*(const float& multiplier) const
{
	return IntVector2((int)floor(x * multiplier), (int)floor(y * multiplier));
}
const IntVector2 IntVector2::operator/(const float& multiplier) const
{
	return IntVector2((int)floor(x / multiplier), (int)floor(y / multiplier));
}
const bool IntVector2::operator==(const IntVector2& other) const
{
	if (other.x == x && other.y == y)
	{
		return true;
	}
	return false;
}
const bool IntVector2::operator!=(const IntVector2& other) const
{
	if (other.x != x || other.y != y)
	{
		return true;
	}
	return false;
}

const bool IntVector2::operator<(const IntVector2& other) const
{
	if (y < other.y)
	{
		return true;
	}
	else if (y > other.y)
	{
		return false;
	}
	else
	{
		return (x < other.x);
	}
}

const bool IntVector2::operator>(const IntVector2& other) const
{
	if (y > other.y)
	{
		return true;
	}
	else if (other.y > y)
	{
		return false;
	}
	else
	{
		return (x > other.x);
	}
}
const bool IntVector2::operator>=(const IntVector2& other) const
{
	if (y >= other.y && x >= other.x)
	{
		return true;
	}
	return false;
}
const bool IntVector2::operator<=(const IntVector2& other) const
{
	if (y <= other.y && x <= other.x)
	{
		return true;
	}
	return false;
}

const void IntVector2::operator+=(const IntVector2& other)
{
	x = x + other.x;
	y = y + other.y;
}
const void IntVector2::operator-=(const IntVector2& other)
{
	x = x - other.x;
	y = y - other.y;
}


const bool IntVector2::operatorLessThanByLength(const IntVector2& other) const
{
	float length = getLength();
	float otherLength = other.getLength();
	if (length < otherLength)
	{
		return true;
	}
	else
	{
		return false;
	}
}