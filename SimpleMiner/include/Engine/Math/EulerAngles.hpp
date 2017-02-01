#pragma once

#ifndef EULERANGLES_HPP
#define EULERANGLES_HPP

class EulerAngles
{
public:

	//constructors
	EulerAngles(const EulerAngles& other);
	EulerAngles(const float& roll = 0.f, const float& pitch = 0.f, const float& yaw = 0.f);

	void setAllAngles(const float& roll, const float& pitch, const float& yaw);

	//operators
	const EulerAngles operator+(const EulerAngles&) const;
	const EulerAngles operator-(const EulerAngles&) const;
	const bool operator==(const EulerAngles&) const;
	const bool operator!=(const EulerAngles&) const;
	const void operator+=(const EulerAngles&);
	const void operator-=(const EulerAngles&);
	const EulerAngles operator*(const float&) const;
	const EulerAngles operator/(const float&) const;

	//variables
	float m_xRoll;
	float m_yPitch;
	float m_zYaw;
};
#endif