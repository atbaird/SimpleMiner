#pragma once
#include "Engine/Math/Vector3.hpp"

#ifndef JETPACK_HPP
#define JETPACK_HPP

class JetPack
{
public:
	JetPack();
	Vector3 PropelUpwards(float deltaSeconds);
	void RecoverFuel(float deltaSeconds);
	float GetRemainingFuel() { return fuel; };
	float GetMaxFuel() { return s_maxFuel; };

private:
	float fuel;
	float secondsSinceLandedAndNotPropelling;
	static const float s_maxFuel;
	static const float s_fuelCostPerSecond;
	static const float s_verticalAccelerationPerFuelExpended;
	static const float s_delayBeforeRegainFuelInSeconds;
};
#endif
