#include "Timer.h"
#include <SDL.h>

KlemmUI::Timer::Timer()
{
	Reset();
}

void KlemmUI::Timer::Reset()
{
	Time = SDL_GetPerformanceCounter();
}

float KlemmUI::Timer::Get() const
{
	Uint64 counterElapsed = SDL_GetPerformanceCounter() - Time;
	return ((float)counterElapsed) / ((float)SDL_GetPerformanceFrequency());
}
