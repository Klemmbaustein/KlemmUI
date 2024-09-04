#include <KlemmUI/Timer.h>

KlemmUI::Timer::Timer()
{
	Reset();
}

#if _WIN32
#include <Windows.h>
void KlemmUI::Timer::Reset()
{
	LARGE_INTEGER time;
	QueryPerformanceCounter(&time);
	Time = *(uint64_t*)&time;
}

float KlemmUI::Timer::Get() const
{
	LARGE_INTEGER time;
	QueryPerformanceFrequency(&time);
	uint64_t Frequency = *(uint64_t*)&time;
	QueryPerformanceCounter(&time);
	uint64_t Now = *(uint64_t*)&time;

	uint64_t counterElapsed = Now - Time;
	return ((float)counterElapsed) / ((float)Frequency);
}
#else
#include <time.h>
#include <iostream>

void KlemmUI::Timer::Reset()
{
	timespec TimeValue;
	clock_gettime(CLOCK_MONOTONIC_RAW, &TimeValue);
	Time = TimeValue.tv_sec;
	Time *= 1000000000LL;
	Time += TimeValue.tv_nsec;
}

float KlemmUI::Timer::Get() const
{
	timespec TimeValue;
	clock_gettime(CLOCK_MONOTONIC_RAW, &TimeValue);
	uint64_t NewTime = TimeValue.tv_sec;
	NewTime *= 1000000000LL;
	NewTime += TimeValue.tv_nsec;

	uint64_t counterElapsed = NewTime - Time;
	return ((float)counterElapsed) / (1000000000.0f);
}

#endif
