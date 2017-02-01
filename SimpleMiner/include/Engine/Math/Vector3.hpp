#pragma once

#ifndef VECTOR3_HPP
#define VECTOR3_HPP

class Vector3
{
public:
	Vector3(const Vector3& other);
	Vector3(const float& _x = 0.f, const float& _y = 0.f, const float& _z = 0.f);

	//setters
	void SetXYZ(const float& _x, const float& _y, const float& _z);

	//getters
	void GetXYZ(float& x_out, float& y_out, float& z_out) const;
	const float GetLength() const;

	//operators
	const float normalize();
	const void floorAll();
	const void ceilAll();
	const void roundAll();
	const void absAll();
	const Vector3 operator+(const Vector3&) const;
	const Vector3 operator-(const Vector3&) const;
	const Vector3 operator*(const float&) const;
	const Vector3 operator/(const float&) const;
	const bool operator==(const Vector3&) const;
	const bool operator!=(const Vector3&) const;
	const void operator+=(const Vector3&);
	const void operator-=(const Vector3&);

	//special functions
	const Vector3 crossProduct(const Vector3&) const;
	const Vector3 projection(const Vector3&) const;
	const float dotProduct(const Vector3& other) const;
	const bool orthogonalTest(const Vector3&) const;

	//variables
	float x;
	float y;
	float z;
};
#endif // !VECTOR3_HPP
