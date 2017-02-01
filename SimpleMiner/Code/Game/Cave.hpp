#pragma once
#include "Game/CavePoint.hpp"
#include "Engine/Math/EulerAngles.hpp"
#include <deque>

#ifndef CAVE_HPP
#define CAVE_HPP

class Cave
{
public:
	Cave(Vector3 position = Vector3(0.f,0.f,0.f));
	static Cave* GenerateCave(float x, float y);
	static float GetCavePerlinNoise(float x, float y);
	static bool DetermineIfCave(float x, float y);

	Vector3 m_caveCenter;
	std::deque<CavePoint> m_cavePath;

private:
	float ComputerWidthPerPoint(Vector3 cavePointPosition) const;
	EulerAngles GetPitchYawToAdd(float x, float y, float z) const;
	void ClampEulerAngles(EulerAngles& angles);
	const Vector3 GetForwardXY(const EulerAngles& angles) const;
	const Vector3 GetForwardXYZ(const EulerAngles& angles) const;

public:
	int m_trueNumOfPoints;

	static const float s_caveMaxRadius;
	static const float s_minCaveWidth;
	static const float s_maxAdditiveCaveWidth;
	static const int s_minNumCavePoints;
	static const int s_maxNumAdditiveCavePoints;
	static const float s_isCaveIfPerlinGreaterThanThis;
	static const float s_distanceOutToMoveFromCavePointMin;
	static const int s_distanceOutToTestChunks;
};
#endif