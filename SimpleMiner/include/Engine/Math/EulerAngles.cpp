#include "Engine/Math/EulerAngles.hpp"

//constructors
EulerAngles::EulerAngles(const EulerAngles& other)
: m_xRoll(other.m_xRoll), m_yPitch(other.m_yPitch), m_zYaw(other.m_zYaw)
{

}
EulerAngles::EulerAngles(const float& roll, const float& pitch, const float& yaw)
: m_xRoll(roll), m_yPitch(pitch), m_zYaw(yaw)
{
}

void EulerAngles::setAllAngles(const float& roll, const float& pitch, const float& yaw)
{
	m_xRoll = roll;
	m_yPitch = pitch;
	m_zYaw = yaw;
}

//operators
const EulerAngles EulerAngles::operator+(const EulerAngles& other) const
{
	return EulerAngles(m_xRoll + other.m_xRoll, m_yPitch + other.m_yPitch, m_zYaw + other.m_zYaw);
}
const EulerAngles EulerAngles::operator-(const EulerAngles& other) const
{
	return EulerAngles(m_xRoll - other.m_xRoll, m_yPitch - other.m_yPitch, m_zYaw - other.m_zYaw);
}

const EulerAngles EulerAngles::operator*(const float& multiplier) const
{
	return EulerAngles(m_xRoll * multiplier, m_yPitch * multiplier, m_zYaw * multiplier);
}

const EulerAngles EulerAngles::operator/(const float& multiplier) const
{
	return EulerAngles(m_xRoll / multiplier, m_yPitch / multiplier, m_zYaw / multiplier);

}

const bool EulerAngles::operator==(const EulerAngles& other) const
{
	if (m_xRoll == other.m_xRoll && m_yPitch == other.m_yPitch && m_zYaw == other.m_zYaw)
	{
		return true;
	}
	return false;
}
const bool EulerAngles::operator!=(const EulerAngles& other) const
{
	if (m_xRoll != other.m_xRoll || m_yPitch != other.m_yPitch || m_zYaw != other.m_zYaw)
	{
		return true;
	}
	return false;
}
const void EulerAngles::operator+=(const EulerAngles& other)
{
	m_xRoll = m_xRoll + other.m_xRoll;
	m_yPitch = m_yPitch + other.m_yPitch;
	m_zYaw = m_zYaw + other.m_zYaw;
}
const void EulerAngles::operator-=(const EulerAngles& other)
{
	m_xRoll = m_xRoll - other.m_xRoll;
	m_yPitch = m_yPitch - other.m_yPitch;
	m_zYaw = m_zYaw - other.m_zYaw;
}