#pragma once

#ifndef VECTOR4_HPP
#define VECTOR4_HPP

class Vector4
{
public:
	Vector4(const Vector4& other);
	Vector4(const float& _w = 0.f, const float& _x = 0.f, const float& _y = 0.f, const float& _z = 0.f);

	//setters
	void SetWXYZ(const float& _w, const float& _x, const float& _y, const float& _z);

	//getters
	void GetWXYZ(float& w_out, float& x_out, float& y_out, float& z_out) const;
	const float GetLength() const;

	//operators
	const float normalize();
	const void floorAll();
	const void ceilAll();
	const void roundAll();
	const void absAll();
	const Vector4 operator+(const Vector4&) const;
	const Vector4 operator-(const Vector4&) const;
	const Vector4 operator*(const float&) const;
	const Vector4 operator/(const float&) const;
	const bool operator==(const Vector4&) const;
	const bool operator!=(const Vector4&) const;
	const void operator+=(const Vector4&);
	const void operator-=(const Vector4&);

	//special functions
	const Vector4 projection(const Vector4&) const;
	const float dotProduct(const Vector4& other) const;
	const bool orthogonalTest(const Vector4&) const;

	//variables
	float w;
	float x;
	float y;
	float z;
};
#endif // !VECTOR3_HPP
