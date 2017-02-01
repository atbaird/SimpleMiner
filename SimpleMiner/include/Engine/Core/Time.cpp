//-----------------------------------------------------------------------------------------------
// Time.cpp
//	A simple high-precision time utility function for Windows
//	based on code by Squirrel Eiserloh

//-----------------------------------------------------------------------------------------------
#include "Engine/Core/Time.hpp"
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>


//-----------------------------------------------------------------------------------------------
const double InitializeTime(LARGE_INTEGER& out_initialTime)
{
	//Initializes time
	LARGE_INTEGER countsPerSecond;
	QueryPerformanceFrequency(&countsPerSecond);
	QueryPerformanceCounter(&out_initialTime);
	return(1.0 / static_cast<double>(countsPerSecond.QuadPart));
}


//-----------------------------------------------------------------------------------------------
const double GetCurrentTimeSeconds()
{
	//returns the current time in seconds.
	static LARGE_INTEGER initialTime;
	static double secondsPerCount = InitializeTime(initialTime);
	LARGE_INTEGER currentCount;
	QueryPerformanceCounter(&currentCount);
	LONGLONG elapsedCountsSinceInitialTime = currentCount.QuadPart - initialTime.QuadPart;

	double currentSeconds = static_cast<double>(elapsedCountsSinceInitialTime)* secondsPerCount;
	return currentSeconds;
}


