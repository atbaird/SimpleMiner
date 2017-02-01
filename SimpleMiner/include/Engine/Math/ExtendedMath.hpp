#pragma once
#include "Engine/Math/Vector2.hpp"


const float Interpolate(const float& start, const float& end, const float& fractionTowardEnd);
const float RangeMapFloat(const float& inValue, const float& inStart, const float& inEnd, const float& outStart, const float& outEnd);
const float calcFractionInRange(const float& inValue, const float& rangeStart, const float& rangeEnd);
const float ClampFloat(const float& inValue, const float& min, const float& max);
const float ClampFloatZeroToOne(const float& inValue);
const float ClampFloatNegativeOneToOne(const float& inValue);
const int ClampInt(const int& inValue, const int& min, const int& max);
const int RoundToNearestInt(const float& inValue);

const float DegreesToRadians(const float& degrees);
const float RadiansToDegrees(const float& radians);
const float CosDegrees(const float& degrees);
const float ACosDegrees(const float& val);
const float SinDegrees(const float& degrees);
const float ASinDegrees(const float& val);
const float Atan2Degrees(const Vector2& direction);
const float ComputeShortestAngularDisplaement(const float& fromDegrees, const float& toDegrees);
float FastFloor(float f);
int FastFloorToInt(float f);

//Vector2
void generateVertex_PCTPositionsForPolygon(const Vector2& center, const float& radius, const int& numSides, const float& degreesOffSet, Vector2*& pointsToReturn);
const float CalcDistanceSquared(const Vector2& pos1, const Vector2& pos2);
const float CalcDistance(const Vector2& pos1, const Vector2& pos2);
const bool DoDiscsOverlap(const Vector2& center1, const float& radius1, const Vector2& center2, const float& radius2);

const float GetRandomFloatZeroToOne();
const float GetRandFloatMinToMax(const float& min, const float& max);
const bool CheckRandomChance(const float& chanceForSuccess);
const float SmoothStep(const float& val);

//Pseudo random

inline const float GetPseudoRandomNoise1D(const int& position);
inline const float GetPseudoRandomNoise2D(const int& positionX, const int& positionY);
inline const float GetPseudoRandomNoise3D(const int& positionX, const int& positionY, const int& positionZ);
inline const Vector2 GetPseudoRandomNoiseDirection2D(const int& positionX, const int& positionY);
const float ComputePerlinNoise2D(const Vector2& position, const float& perlinNoiseGridCellSize, const int& numOctaves,
	const float& persistance, const float& amplitude = 1.f, const float& add = 0.f);

//-----------------------------------------------------------------------------------------------
//variables
//
const double TWO_PI = 6.283185307179586476925286766559;
const double PI = 3.1415926535897932384626433832795;
const double HALF_PI = 1.5707963267948966192313216916398;
const double QUARTER_PI = 0.78539816339744830961566084581988;
const double SQRT_2 = 1.4142135623730950488016887242097;
const double SQRT_3 = 1.7320508075688772935274463415059;
const double SQRT_2_OVER_2 = 0.70710678118654752440084436210485;
const double SQRT_3_OVER_2 = 0.86602540378443864676372317075294;
const double SQRT_3_OVER_3 = 0.57735026918962576450914878050196;
const double SQRT_3_OVER_6 = 0.28867513459481288225457439025098;

const float fTWO_PI = 6.283185307179586476925286766559f;
const float fPI = 3.1415926535897932384626433832795f;
const float fHALF_PI = 1.5707963267948966192313216916398f;
const float fQUARTER_PI = 0.78539816339744830961566084581988f;
const float fSQRT_2 = 1.4142135623730950488016887242097f;
const float fSQRT_3 = 1.7320508075688772935274463415059f;
const float fSQRT_2_OVER_2 = 0.70710678118654752440084436210485f;
const float fSQRT_3_OVER_2 = 0.86602540378443864676372317075294f;
const float fSQRT_3_OVER_3 = 0.57735026918962576450914878050196f;
const float fSQRT_3_OVER_6 = 0.28867513459481288225457439025098f;

//-----------------------------------------------------------------------------------------------
// Lookup tables
extern const float g_sinValuesForByteAngles[256];