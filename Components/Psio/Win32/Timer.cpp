#include "../Timer.h"
#include <Windows.h>

namespace Psio
{
	uint64_t Timer::getTicks()
	{
		uint64_t ticks;

		// This is the number of clock ticks since start of windows
		if (!QueryPerformanceCounter((LARGE_INTEGER*)&ticks)) {
			// this returns only milliseconds
			ticks = (UINT64)timeGetTime() * 1000;
		}

		return ticks;
	}
}