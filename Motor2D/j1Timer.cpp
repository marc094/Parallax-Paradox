// ----------------------------------------------------
// j1Timer.cpp
// Fast timer with milisecons precision
// ----------------------------------------------------

#include "j1Timer.h"
#include "SDL\include\SDL_timer.h"

// ---------------------------------------------
j1Timer::j1Timer()
{
	Start();
}

// ---------------------------------------------
void j1Timer::Start()
{
	started_at = SDL_GetTicks();
}

// ---------------------------------------------
uint32 j1Timer::Read() const
{
	return SDL_GetTicks() - started_at;
}

// ---------------------------------------------
float j1Timer::ReadSec() const
{
	return float(SDL_GetTicks() - started_at) / 1000.0f;
}

bool j1Timer::Count(float num)
{
	bool ret = false;
	if (num <= ((SDL_GetTicks() - started_at) / 1000.0f))
	{
		ret = true;
	}

	return ret;
}

void j1Timer::Set(float num)
{
	started_at = num;
}

uint32 j1Timer::Started() const
{
	return started_at;
}