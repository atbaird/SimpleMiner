#include "Engine/Math/ExtendedMath.hpp"
#include "Engine/Math/IntVector2.hpp"
#include <cmath>
#include <stdlib.h>

//-----------------------------------------------------------------------------------------------
// Lookup table for sin() values for 256 evenly distributed angles around the unit circle,
//	where Byte 0 = 0 degrees, Byte 64 = 90 degrees, Byte 128 = 180 degrees, etc.
//
const float g_sinValuesForByteAngles[256] = {
	0.000000000f, 0.024541229f, 0.049067674f, 0.073564564f, 0.098017140f, 0.122410675f, 0.146730474f, 0.170961889f,
	0.195090322f, 0.219101240f, 0.242980180f, 0.266712757f, 0.290284677f, 0.313681740f, 0.336889853f, 0.359895037f,
	0.382683432f, 0.405241314f, 0.427555093f, 0.449611330f, 0.471396737f, 0.492898192f, 0.514102744f, 0.534997620f,
	0.555570233f, 0.575808191f, 0.595699304f, 0.615231591f, 0.634393284f, 0.653172843f, 0.671558955f, 0.689540545f,
	0.707106781f, 0.724247083f, 0.740951125f, 0.757208847f, 0.773010453f, 0.788346428f, 0.803207531f, 0.817584813f,
	0.831469612f, 0.844853565f, 0.857728610f, 0.870086991f, 0.881921264f, 0.893224301f, 0.903989293f, 0.914209756f,
	0.923879533f, 0.932992799f, 0.941544065f, 0.949528181f, 0.956940336f, 0.963776066f, 0.970031253f, 0.975702130f,
	0.980785280f, 0.985277642f, 0.989176510f, 0.992479535f, 0.995184727f, 0.997290457f, 0.998795456f, 0.999698819f,
	1.000000000f, 0.999698819f, 0.998795456f, 0.997290457f, 0.995184727f, 0.992479535f, 0.989176510f, 0.985277642f,
	0.980785280f, 0.975702130f, 0.970031253f, 0.963776066f, 0.956940336f, 0.949528181f, 0.941544065f, 0.932992799f,
	0.923879533f, 0.914209756f, 0.903989293f, 0.893224301f, 0.881921264f, 0.870086991f, 0.857728610f, 0.844853565f,
	0.831469612f, 0.817584813f, 0.803207531f, 0.788346428f, 0.773010453f, 0.757208847f, 0.740951125f, 0.724247083f,
	0.707106781f, 0.689540545f, 0.671558955f, 0.653172843f, 0.634393284f, 0.615231591f, 0.595699304f, 0.575808191f,
	0.555570233f, 0.534997620f, 0.514102744f, 0.492898192f, 0.471396737f, 0.449611330f, 0.427555093f, 0.405241314f,
	0.382683432f, 0.359895037f, 0.336889853f, 0.313681740f, 0.290284677f, 0.266712757f, 0.242980180f, 0.219101240f,
	0.195090322f, 0.170961889f, 0.146730474f, 0.122410675f, 0.098017140f, 0.073564564f, 0.049067674f, 0.024541229f,
	0.000000000f, -0.024541229f, -0.049067674f, -0.073564564f, -0.098017140f, -0.122410675f, -0.146730474f, -0.170961889f,
	-0.195090322f, -0.219101240f, -0.242980180f, -0.266712757f, -0.290284677f, -0.313681740f, -0.336889853f, -0.359895037f,
	-0.382683432f, -0.405241314f, -0.427555093f, -0.449611330f, -0.471396737f, -0.492898192f, -0.514102744f, -0.534997620f,
	-0.555570233f, -0.575808191f, -0.595699304f, -0.615231591f, -0.634393284f, -0.653172843f, -0.671558955f, -0.689540545f,
	-0.707106781f, -0.724247083f, -0.740951125f, -0.757208847f, -0.773010453f, -0.788346428f, -0.803207531f, -0.817584813f,
	-0.831469612f, -0.844853565f, -0.857728610f, -0.870086991f, -0.881921264f, -0.893224301f, -0.903989293f, -0.914209756f,
	-0.923879533f, -0.932992799f, -0.941544065f, -0.949528181f, -0.956940336f, -0.963776066f, -0.970031253f, -0.975702130f,
	-0.980785280f, -0.985277642f, -0.989176510f, -0.992479535f, -0.995184727f, -0.997290457f, -0.998795456f, -0.999698819f,
	-1.000000000f, -0.999698819f, -0.998795456f, -0.997290457f, -0.995184727f, -0.992479535f, -0.989176510f, -0.985277642f,
	-0.980785280f, -0.975702130f, -0.970031253f, -0.963776066f, -0.956940336f, -0.949528181f, -0.941544065f, -0.932992799f,
	-0.923879533f, -0.914209756f, -0.903989293f, -0.893224301f, -0.881921264f, -0.870086991f, -0.857728610f, -0.844853565f,
	-0.831469612f, -0.817584813f, -0.803207531f, -0.788346428f, -0.773010453f, -0.757208847f, -0.740951125f, -0.724247083f,
	-0.707106781f, -0.689540545f, -0.671558955f, -0.653172843f, -0.634393284f, -0.615231591f, -0.595699304f, -0.575808191f,
	-0.555570233f, -0.534997620f, -0.514102744f, -0.492898192f, -0.471396737f, -0.449611330f, -0.427555093f, -0.405241314f,
	-0.382683432f, -0.359895037f, -0.336889853f, -0.313681740f, -0.290284677f, -0.266712757f, -0.242980180f, -0.219101240f,
	-0.195090322f, -0.170961889f, -0.146730474f, -0.122410675f, -0.098017140f, -0.073564564f, -0.049067674f, -0.024541229f
};

const float Interpolate(const float& start, const float& end, const float& fractionTowardEnd)
{
	//fix me.
	return 0.f;
}
const float RangeMapFloat(const float& inValue, const float& inStart, const float& inEnd, const float& outStart, const float& outEnd)
{
	//fix me.
	return 0.f;
}
const float calcFractionInRange(const float& inValue, const float& rangeStart, const float& rangeEnd)
{
	//fix me.
	return 0.f;
}
const float ClampFloat(const float& inValue, const float& min, const float& max)
{
	if (inValue < min)
	{
		return min;
	}
	else if (inValue > max)
	{
		return max;
	}
	return inValue;
}
const float ClampFloatZeroToOne(const float& inValue)
{
	if (inValue < 0.f)
	{
		return 0.f;
	}
	else if (inValue > 1.f)
	{
		return 1.f;
	}
	return inValue;

}
const float ClampFloatNegativeOneToOne(const float& inValue)
{
	if (inValue < -1.f)
	{
		return -1.f;
	}
	if (inValue > 1.f)
	{
		return 1.f;
	}
	return inValue;
}
const int ClampInt(const int& inValue, const int& min, const int& max)
{
	if (inValue < min)
	{
		return min;
	}
	if (inValue > max)
	{
		return max;
	}
	return inValue;
}
const int RoundToNearestInt(const float& inValue)
{
	return (int)round(inValue);
}



const float DegreesToRadians(const float& degreesOffSet)
{
	//returns a degrees angle in the form of a radian angle
	return ((float)(degreesOffSet * PI)) / 180.f;
}

const float RadiansToDegrees(const float& RadiansOffSet)
{
	//returns a radian angle in the form a degrees angle
	return (float)((RadiansOffSet * 180.f) / PI);
}
const float CosDegrees(const float& degrees)
{
	return cos(DegreesToRadians(degrees));
}

const float ACosDegrees(const float& val)
{
	return RadiansToDegrees(acos(val));
}
const float SinDegrees(const float& degrees)
{
	return sin(DegreesToRadians(degrees));
}
const float ASinDegrees(const float& val)
{
	return RadiansToDegrees(asin(val));
}

const float Atan2Degrees(const Vector2& direction)
{
	return RadiansToDegrees(atan2(direction.y, direction.x));
}
const float ComputeShortestAngularDisplaement(const float& fromCurrent, const float& toGoal)
{
	float difference = toGoal - fromCurrent;
	while (difference > 180.f)
	{
		difference -= 360.f;
	}
	while (difference < -180.f)
	{
		difference += 360.f;
	}
	return difference;
}



void generateVertex_PCTPositionsForPolygon(const Vector2& center, const float& radius, const int& numSides, const float& degreesOffSet, Vector2*& pointsToReturn)
{
	//generates Vertex_PCT positions for a polygon shape
	if (pointsToReturn != nullptr)
	{
		delete pointsToReturn;
		pointsToReturn = nullptr;
	}
	pointsToReturn = new Vector2[numSides];
	const float radiansTotal = (float)(2.f * PI);
	const float radiansPerSide = radiansTotal / numSides;

	int index = 0;
	for (float radians = 0.f; radians < radiansTotal; radians += radiansPerSide) {
		//float rotatedRadians = (0.5f * pi) + radians;
		float rotatedRadians = radians + DegreesToRadians(degreesOffSet);
		//float rotatedRadians = radians + DegreesToRadians(degreesOffSet);
		float x = (cos(rotatedRadians) * radius) + center.x;
		float y = (sin(rotatedRadians) * radius) + center.y;
		pointsToReturn[index] = Vector2(x, y);
		index++;
	}
}
const float CalcDistanceSquared(const Vector2& pos1, const Vector2& pos2)
{
	Vector2 difference = pos2 - pos1;
	return (difference.y * difference.y) + (difference.x * difference.x);
}
const float CalcDistance(const Vector2& pos1, const Vector2& pos2)
{
	Vector2 difference = pos2 - pos1;
	return sqrt((difference.y * difference.y) + (difference.x * difference.x));
}
const bool DoDiscsOverlap(const Vector2& center1, const float& radius1, const Vector2& center2, const float& radius2)
{
	float calcDistance = CalcDistance(center1, center2);
	float addedRadi = radius1 + radius2;
	if (abs(calcDistance) < addedRadi)
	{
		return true;
	}
	return false;
}


const float GetRandomFloatZeroToOne()
{
	return (float)(rand() % 2);
}
const float GetRandFloatMinToMax(const float& min, const float& max)
{
	return min + (rand() % (int)(max - min));
}
const bool CheckRandomChance(const float& chanceForSuccess)
{
	int ran = rand() / 100;
	if (ran > chanceForSuccess)
	{
		return true;
	}
	return false;
}


const float SmoothStep(const float& val)
{
	//3x^2 - 2x^3
	float val2 = (val * val);
	return ((val2)* 3) - ((val2 * val) * 2);
}

//pseudo random
//-----------------------------------------------------------------------------------------------
inline const float GetPseudoRandomNoise1D(const int& position)
{
	const float ONE_OVER_MAX_POSITIVE_INT = (1.f / 2147483648.f);
	int bits = (position << 13) ^ position;
	int pseudoRandomPositiveInt = (bits * ((bits * bits * 15731) + 789221) + 1376312589) & 0x7fffffff;
	float pseudoRandomFloatZeroToOne = ONE_OVER_MAX_POSITIVE_INT * (float)pseudoRandomPositiveInt;
	return pseudoRandomFloatZeroToOne;
}


//-----------------------------------------------------------------------------------------------
const float GetPseudoRandomNoise2D(const int& positionX, const int& positionY)
{
	const float ONE_OVER_MAX_POSITIVE_INT = (1.f / 2147483648.f);
	int position = positionX + (positionY * 57);
	int bits = (position << 13) ^ position;
	int pseudoRandomPositiveInt = (bits * ((bits * bits * 15731) + 789221) + 1376312589) & 0x7fffffff;
	float pseudoRandomFloatZeroToOne = ONE_OVER_MAX_POSITIVE_INT * (float)pseudoRandomPositiveInt;
	return pseudoRandomFloatZeroToOne;
}


//-----------------------------------------------------------------------------------------------
const float GetPseudoRandomNoise3D(const int& positionX, const int& positionY, const int& positionZ)
{
	const float ONE_OVER_MAX_POSITIVE_INT = (1.f / 2147483648.f);
	int seed = positionX + (positionY * 57) + (positionZ * 3023);
	int bits = (seed << 13) ^ seed;
	int pseudoRandomPositiveInt = (bits * ((bits * bits * 15731) + 789221) + 1376312589) & 0x7fffffff;
	float pseudoRandomFloatZeroToOne = ONE_OVER_MAX_POSITIVE_INT * (float)pseudoRandomPositiveInt;
	return pseudoRandomFloatZeroToOne;
}


//-----------------------------------------------------------------------------------------------
const Vector2 GetPseudoRandomNoiseDirection2D(const int& xPosition, const int& yPosition)
{
	const float ONE_OVER_MAX_POSITIVE_INT = (1.f / 2147483648.f);
	const float SCALE_FACTOR = ONE_OVER_MAX_POSITIVE_INT * 360.f;
	int position = xPosition + (yPosition * 57);
	int bits = (position << 13) ^ position;
	int pseudoRandomPositiveInt = (bits * ((bits * bits * 15731) + 789221) + 1376312589) & 0x7fffffff;
	float pseudoRandomDegrees = SCALE_FACTOR * (float)pseudoRandomPositiveInt;

	// #TODO: Rewrite this to use the randomized int to look up Vector2 from a (small) cos/sin
	//	table; vectors don't need to be super-precise, and we certainly don't want to pay full
	//	price for cos/sin if this is merely going to be used for, say, Perlin noise gradients.
	//	Note that cos/sin are typically fast on GPUs so this can probably stand in shader code.
	return Vector2(CosDegrees(pseudoRandomDegrees), SinDegrees(pseudoRandomDegrees));
}
//-----------------------------------------------------------------------------------------------
// Computes a random Perlin noise value in the range [-1,1] based on a 2D input <position> and
//	various Perlin noise parameters.
//
//	<perlinNoiseGridCellSize>: Noise density.  Larger values produce longer wavelength noise
//		(e.g. gentle, sweeping hills).
//	<numOctaves>: 0 is flat, 1 is simple smoothed noise. Values of 2+ add one or more additional
//		"octave" harmonics.  Each additional octave has double the frequency/density but only a
//		fraction of the amplitude of the base noise.
//	<persistence>: The fraction of amplitude of each subsequent octave, based on the amplitude of
//		the previous octave.  For example, with a persistence of 0.3, each octave is only 30% as
//		strong as the previous octave.
//
const float ComputePerlinNoise2D(const Vector2& position, const float& perlinNoiseGridCellSize, const int& numOctaves, const float& persistence, 
	const float& amplitude, const float& add)
{
	float totalPerlinNoise = 0.f;
	float currentOctaveAmplitude = 1.f;
	float totalMaxAmplitude = 0.f;
	float perlinGridFrequency = (1.f / perlinNoiseGridCellSize);
	for (int octaveNumber = 0; octaveNumber < numOctaves; ++octaveNumber)
	{
		Vector2 perlinPosition(position.x * perlinGridFrequency, position.y * perlinGridFrequency);
		Vector2 perlinPositionFloor((float)floor(perlinPosition.x), (float)floor(perlinPosition.y));
		IntVector2 perlinCell((int)perlinPositionFloor.x, (int)perlinPositionFloor.y);
		Vector2 perlinPositionUV = perlinPosition - perlinPositionFloor;
		Vector2 perlinPositionAntiUV(perlinPositionUV.x - 1.f, perlinPositionUV.y - 1.f);
		float eastWeight = SmoothStep(perlinPositionUV.x);
		float northWeight = SmoothStep(perlinPositionUV.y);
		float westWeight = 1.f - eastWeight;
		float southWeight = 1.f - northWeight;

		Vector2 southwestNoiseGradient = GetPseudoRandomNoiseDirection2D(perlinCell.x, perlinCell.y);
		Vector2 southeastNoiseGradient = GetPseudoRandomNoiseDirection2D(perlinCell.x + 1, perlinCell.y);
		Vector2 northeastNoiseGradient = GetPseudoRandomNoiseDirection2D(perlinCell.x + 1, perlinCell.y + 1);
		Vector2 northwestNoiseGradient = GetPseudoRandomNoiseDirection2D(perlinCell.x, perlinCell.y + 1);

		float southwestDot = southwestNoiseGradient.dotProduct(perlinPositionUV);
		float southeastDot = southeastNoiseGradient.dotProduct(Vector2(perlinPositionAntiUV.x, perlinPositionUV.y));
		float northeastDot = northeastNoiseGradient.dotProduct(perlinPositionAntiUV);
		float northwestDot = northwestNoiseGradient.dotProduct(Vector2(perlinPositionUV.x, perlinPositionAntiUV.y));

		float southBlend = (eastWeight * southeastDot) + (westWeight * southwestDot);
		float northBlend = (eastWeight * northeastDot) + (westWeight * northwestDot);
		float fourWayBlend = (southWeight * southBlend) + (northWeight * northBlend);
		float perlinNoiseForThisOctave = currentOctaveAmplitude * fourWayBlend;

		totalPerlinNoise += perlinNoiseForThisOctave;
		perlinGridFrequency *= 2.f;
		totalMaxAmplitude += currentOctaveAmplitude;
		currentOctaveAmplitude *= persistence;
	}

	if (totalMaxAmplitude != 0.f)
		totalPerlinNoise /= totalMaxAmplitude;

	return (totalPerlinNoise * amplitude) + add;
}

float FastFloor(float f)
{
	if (f >= 0.f)
		return (float)(int)f;
	else
	{
		float f2 = (float)(int)f;
		if (f == f2)
			return f2;
		else
			return f2 - 1.f;
	}
}

int FastFloorToInt(float f)
{
	if (f >= 0.f)
		return (int)f;
	else
	{
		int i = (int)f;
		float f2 = (float)i;
		if (f == f2)
			return i;
		else
			return i - 1;
	}
}