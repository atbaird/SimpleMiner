#include "Engine/Math/Vector4.hpp"
#include <cmath>

//constructors

Vector4::Vector4(const Vector4& other)
: w(other.w)
, x(other.x)
, y(other.y)
, z(other.z)
{
}
Vector4::Vector4(const float& _w, const float& _x, const float& _y, const float& _z)
: w(_w)
, x(_x)
, y(_y)
, z(_z)
{
}



//setters
void Vector4::SetWXYZ(const float& _w, const float& _x, const float& _y, const float& _z)
{
	w = _w;
	x = _x;
	y = _y;
	z = _z;
}

//getters

void Vector4::GetWXYZ(float& w_out, float& x_out, float& y_out, float& z_out) const
{
	w_out = w;
	x_out = x;
	y_out = y;
	z_out = z;
}
const float Vector4::GetLength() const
{
	return sqrt((w * w) + (x * x) + (y * y) + (z * z));
}

//operations
const float Vector4::normalize()
{
	//returns a Vector2 of this Vector2 but normalized
	//Consider this as "what direction is up" from the Vertex_PCT.
	//can average all the normals of an object's a shape to get the up vector
	//need to interpolate
	float length = sqrt((w * w) + (x * x) + (y * y) + (z * z));
	if (length != 0)
	{
		w /= length;
		x /= length;
		y /= length;
		z /= length;
	}
	return length;
}

const void Vector4::floorAll()
{
	w = floor(w);
	x = floor(x);
	y = floor(y);
	z = floor(z);
}

const void Vector4::ceilAll()
{
	w = ceil(w);
	x = ceil(x);
	y = ceil(y);
	z = ceil(z);
}
const void Vector4::roundAll()
{
	w = round(w);
	x = round(x);
	y = round(y);
	z = round(z);
}
const void Vector4::absAll()
{
	w = abs(w);
	x = abs(x);
	y = abs(y);
	z = abs(z);
}
const Vector4 Vector4::operator+(const Vector4& other) const
{
	return Vector4(w + other.w, x + other.x, y + other.y, z + other.z);
}
const Vector4 Vector4::operator-(const Vector4& other) const
{
	return Vector4(w - other.w, x - other.x, y - other.y, z - other.z);
}
const Vector4 Vector4::operator*(const float& theFloat) const
{
	return Vector4(w * theFloat, x * theFloat, y * theFloat, z * theFloat);
}

const Vector4 Vector4::operator/(const float& theFloat) const
{
	return Vector4(w / theFloat, x / theFloat, y / theFloat, z / theFloat);
}

const void Vector4::operator+=(const Vector4& other)
{
	w = w + other.w;
	x = x + other.x;
	y = y + other.y;
	z = z + other.z;
}

const void Vector4::operator-=(const Vector4& other)
{
	w = w - other.w;
	x = x - other.x;
	y = y - other.y;
	z = z - other.z;
}


const bool Vector4::operator==(const Vector4& other) const
{
	if (other.w == w && other.x == x && other.y == y && other.z == z)
	{
		return true;
	}
	return false;
}

const bool Vector4::operator!=(const Vector4& other) const
{
	if (other.w != w || other.x != x || other.y != y || other.z != z)
	{
		return true;
	}
	return false;
}



//special functions
const float Vector4::dotProduct(const Vector4& other) const
{
	return (other.w * w) + (other.x * x) + (other.y * y) + (other.z * z);
}

const bool Vector4::orthogonalTest(const Vector4& other) const
{
	float test = dotProduct(other);
	if (test == 0)
	{
		return true;
	}
	return false;
}

const Vector4 Vector4::projection(const Vector4& other) const
{
	float dotProductQP = (other.w * w) + (other.x * x) + (other.y * y) + (other.z * z);
	float dotProductPP = (w * w) + (x * x) + (y * y) + (z * z);
	float fraction = dotProductQP / dotProductPP;
	return Vector4(w, x, y, z) * fraction;
}