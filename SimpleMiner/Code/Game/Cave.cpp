#include "Game/Cave.hpp"
#include "Engine/Math/ExtendedMath.hpp"

const float Cave::s_caveMaxRadius = 80.0f;
const float Cave::s_minCaveWidth = 1.0f;
const float Cave::s_maxAdditiveCaveWidth = 14.0f; //max of 8, min of 5 radius.
const int Cave::s_minNumCavePoints = 10;
const int Cave::s_maxNumAdditiveCavePoints = 30;
const float Cave::s_isCaveIfPerlinGreaterThanThis = 0.65f;
const int Cave::s_distanceOutToTestChunks = 10;
const float Cave::s_distanceOutToMoveFromCavePointMin = 16.f;
Cave::Cave(Vector3 position)
: m_caveCenter(position),
m_trueNumOfPoints(s_minNumCavePoints)
{
	float numPointsToAdd = ComputePerlinNoise2D(Vector2(floor(position.x), floor(position.y)), 200.f, 8, 500.f, (float)s_maxNumAdditiveCavePoints, s_maxNumAdditiveCavePoints / 2.f);
	m_trueNumOfPoints += (int)round(numPointsToAdd);

	m_cavePath.push_back(CavePoint(m_caveCenter, ComputerWidthPerPoint(m_caveCenter)));
	EulerAngles startPosAngle = GetPitchYawToAdd(m_caveCenter.x, m_caveCenter.y, m_caveCenter.z);
	ClampEulerAngles(startPosAngle);

	Vector3 startPositionPosDir = m_caveCenter;
	Vector3 startPositionNegDir = m_caveCenter;
	EulerAngles anglesPosDir = startPosAngle;
	EulerAngles anglesNegDir = GetPitchYawToAdd(m_caveCenter.x, m_caveCenter.z, m_caveCenter.y) * -1.f;

	for (int i = 0; i < (m_trueNumOfPoints - 1); i += 2)
	{
		Vector3 amountToAddForward = GetForwardXYZ(anglesPosDir) * s_distanceOutToMoveFromCavePointMin;
		Vector3 amountToAddBackward = GetForwardXYZ(anglesNegDir) * s_distanceOutToMoveFromCavePointMin;
		startPositionPosDir += amountToAddForward;
		startPositionNegDir += amountToAddBackward;
		m_cavePath.push_front(CavePoint(startPositionPosDir, ComputerWidthPerPoint(startPositionPosDir)));
		m_cavePath.push_back(CavePoint(startPositionNegDir, ComputerWidthPerPoint(startPositionNegDir)));

		EulerAngles addPositive = GetPitchYawToAdd(startPositionPosDir.x, startPositionPosDir.y, startPositionPosDir.z);
		EulerAngles addNegative = GetPitchYawToAdd(startPositionNegDir.x, startPositionNegDir.y, startPositionNegDir.z);
		ClampEulerAngles(addPositive);
		ClampEulerAngles(addNegative);
		anglesPosDir += addPositive;
		anglesNegDir += addNegative;
	}
}
const Vector3 Cave::GetForwardXYZ(const EulerAngles& angles) const
{
	Vector3 forwardXYZ = GetForwardXY(angles);
	float cosOrien = CosDegrees(angles.m_yPitch);
	forwardXYZ.x = forwardXYZ.x * cosOrien;
	forwardXYZ.y = forwardXYZ.y * cosOrien;
	forwardXYZ.z = SinDegrees(-angles.m_yPitch);
	return forwardXYZ;
}
const Vector3 Cave::GetForwardXY(const EulerAngles& angles) const
{
	float cosYaw = CosDegrees(angles.m_zYaw);
	float sinYaw = SinDegrees(angles.m_zYaw);
	return Vector3(cosYaw, sinYaw, 0.f);
}
Cave* Cave::GenerateCave(float x, float y)
{
	float zHeight = ComputePerlinNoise2D(Vector2(x , y), 10.f, 2, 10.f, 128.f, 32.f);
	Cave* newCave = new Cave(Vector3(x,y, zHeight));
	return newCave;
}
float Cave::GetCavePerlinNoise(float x, float y)
{
	return ComputePerlinNoise2D(Vector2(x, y), 6.f, 1, 100.f);
}
bool Cave::DetermineIfCave(float x, float y)
{
	float perlinNoise = (GetCavePerlinNoise(x, y) * 0.5f) + 0.5f;
	return (perlinNoise > s_isCaveIfPerlinGreaterThanThis);
}

float Cave::ComputerWidthPerPoint(Vector3 cavePointPosition) const
{
	return ComputePerlinNoise2D(Vector2(floor(cavePointPosition.x / s_distanceOutToMoveFromCavePointMin), floor(cavePointPosition.y / s_distanceOutToMoveFromCavePointMin)), 6.f,
		8, 450.f, (s_maxAdditiveCaveWidth / 2.f), (s_minCaveWidth + (s_maxAdditiveCaveWidth / 2.f)));
}
EulerAngles Cave::GetPitchYawToAdd(float x, float y, float z) const
{
	EulerAngles amountToAdd = EulerAngles(0.f, 0.f, 0.f);
	amountToAdd.m_zYaw = ComputePerlinNoise2D(Vector2(x, y), 100.f, 6, 500.f, 80.f);
	amountToAdd.m_yPitch = ComputePerlinNoise2D(Vector2(x, z), 100.f, 6, 500.f, 80.f);
	return amountToAdd;
}
void Cave::ClampEulerAngles(EulerAngles& angles)
{

	if (angles.m_yPitch > 89.f)
	{
		angles.m_yPitch = 89.f;
	}
	else if (angles.m_yPitch < -89.f)
	{
		angles.m_yPitch = -89.f;
	}
	if (angles.m_zYaw > 180.f)
	{
		angles.m_zYaw = angles.m_zYaw - 360.f;
	}
	else if (angles.m_zYaw < -180.f)
	{
		angles.m_zYaw = angles.m_zYaw + 360.f;
	}
	if (angles.m_xRoll > 180.f)
	{
		angles.m_xRoll = angles.m_xRoll - 360.f;
	}
	else if (angles.m_xRoll < -180.f)
	{
		angles.m_xRoll = angles.m_xRoll + 360.f;
	}
}