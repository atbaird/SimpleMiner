#pragma once

#ifndef INTVECTOR3_HPP
#define INTVECTOR3_HPP
class IntVector3
{
public:
	IntVector3(const int& _x = 0, const int& _y = 0, const int& _z = 0);
	IntVector3(const IntVector3& other);
	
	const float getLength() const;

	const IntVector3 operator+(const IntVector3&) const;
	const IntVector3 operator-(const IntVector3&) const;
	const IntVector3 operator*(const float&) const;
	const IntVector3 operator/(const float&) const;
	const bool operator==(const IntVector3&) const;
	const bool operator<(const IntVector3&) const;
	const bool operator!=(const IntVector3&) const;
	const void operator+=(const IntVector3&);
	const void operator-=(const IntVector3&);
	const bool operatorLessThanByLength(const IntVector3&) const;

	int x;
	int y;
	int z;
};

#endif // !INTVECTOR3_HPP

