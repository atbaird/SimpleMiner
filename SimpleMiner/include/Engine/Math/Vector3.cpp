#include "Engine/Math/Vector3.hpp"
#include <cmath>

//constructors

Vector3::Vector3(const Vector3& other)
: x(other.x)
, y(other.y)
, z(other.z)
{
}
Vector3::Vector3(const float& _x, const float& _y, const float& _z)
: x(_x)
, y(_y)
,z(_z)
{
}



//setters
void Vector3::SetXYZ(const float& _x, const float& _y, const float& _z)
{
	x = _x;
	y = _y;
	z = _z;
}

//getters

void Vector3::GetXYZ(float& x_out, float& y_out, float& z_out) const
{
	x_out = x;
	y_out = y;
	z_out = z;
}
const float Vector3::GetLength() const
{
	return sqrt((x * x) + (y * y) + (z * z));
}

//operations
const float Vector3::normalize()
{
	//returns a Vector2 of this Vector2 but normalized
	//Consider this as "what direction is up" from the Vertex_PCT.
	//can average all the normals of an object's a shape to get the up vector
	//need to interpolate
	float length = sqrt((x * x) + (y * y) + (z * z));
	if (length != 0)
	{
		x /= length;
		y /= length;
		z /= length;
	}
	return length;
}

const void Vector3::floorAll()
{
	x = floor(x);
	y = floor(y);
	z = floor(z);
}

const void Vector3::ceilAll()
{
	x = ceil(x);
	y = ceil(y);
	z = ceil(z);
}
const void Vector3::roundAll()
{
	x = round(x);
	y = round(y);
	z = round(z);
}
const void Vector3::absAll()
{
	x = abs(x);
	y = abs(y);
	z = abs(z);
}
const Vector3 Vector3::operator+(const Vector3& other) const
{
	return Vector3(x + other.x, y + other.y, z + other.z);
}
const Vector3 Vector3::operator-(const Vector3& other) const
{
	return Vector3(x - other.x, y - other.y, z - other.z);
}
const Vector3 Vector3::operator*(const float& theFloat) const
{
	return Vector3(x * theFloat, y * theFloat, z * theFloat);
}

const Vector3 Vector3::operator/(const float& theFloat) const
{
	return Vector3(x / theFloat, y / theFloat, z / theFloat);
}

const void Vector3::operator+=(const Vector3& other)
{
	x = x + other.x;
	y = y + other.y;
	z = z + other.z;
}

const void Vector3::operator-=(const Vector3& other)
{
	x = x - other.x;
	y = y - other.y;
	z = z - other.z;
}


const bool Vector3::operator==(const Vector3& other) const
{
	if (other.x == x && other.y == y && other.z == z)
	{
		return true;
	}
	return false;
}

const bool Vector3::operator!=(const Vector3& other) const
{
	if (other.x != x || other.y != y || other.z != z)
	{
		return true;
	}
	return false;
}



//special functions
const float Vector3::dotProduct(const Vector3& other) const
{
	return (other.x * x) + (other.y * y) + (other.z * z);
}

const bool Vector3::orthogonalTest(const Vector3& other) const
{
	float test = dotProduct(other);
	if (test == 0)
	{
		return true;
	}
	return false;
}

const Vector3 Vector3::crossProduct(const Vector3& other) const
{
	return Vector3((y * other.z) - (z * other.y), (z * other.x) - (x * other.z), (x * other.y) - (y * other.x));
}
const Vector3 Vector3::projection(const Vector3& other) const
{
	float dotProductQP = (other.x * x) + (other.y * y) + (other.z * z);
	float dotProductPP = (x * x) + (y * y) + (z * z);
	float fraction = dotProductQP / dotProductPP;
	return Vector3(x, y, z) * fraction;
}