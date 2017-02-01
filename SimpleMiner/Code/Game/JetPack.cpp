#include "Game/JetPack.hpp"

const float JetPack::s_verticalAccelerationPerFuelExpended = 1.0f;
const float JetPack::s_maxFuel = 100.f;
const float JetPack::s_fuelCostPerSecond = 6.0f;
const float JetPack::s_delayBeforeRegainFuelInSeconds = 2.0f;

JetPack::JetPack()
: fuel(s_maxFuel),
secondsSinceLandedAndNotPropelling(0.f)
{
}

Vector3 JetPack::PropelUpwards(float deltaSeconds)
{
	secondsSinceLandedAndNotPropelling = 0.f;
	float fuelCost = s_fuelCostPerSecond * deltaSeconds;
	if ((fuel - fuelCost) < 0.f)
	{
		fuelCost = fuel;
	}
	fuel -= fuelCost;
	if (fuel < 0.f)
	{
		fuel = 0.f;
	}

	Vector3 propulsion = Vector3(0.f, 0.f, fuelCost * s_verticalAccelerationPerFuelExpended);

	return propulsion;
}
void JetPack::RecoverFuel(float deltaSeconds)
{
	secondsSinceLandedAndNotPropelling += deltaSeconds;
	if (secondsSinceLandedAndNotPropelling >= s_delayBeforeRegainFuelInSeconds)
	{
		fuel += (deltaSeconds * (s_fuelCostPerSecond / 2.f));
		if (fuel > s_maxFuel)
		{
			fuel = s_maxFuel;
		}
	}
}