#include <Psio/Timer.h>

#include <Windows.h>

namespace psio
{
	uint64_t Timer::getTicks()
	{
		LARGE_INTEGER ticks;
		static uint64_t PCFreq = 0;

		if (!PCFreq) {
			LARGE_INTEGER li;
			if (!QueryPerformanceFrequency(&li)) {
				// QueryPerformanceFrequency failed!
				abort();
				return 0;
			}
			PCFreq = li.QuadPart;
		}

		// This is the number of clock ticks since start of windows
		if (!QueryPerformanceCounter((LARGE_INTEGER*)&ticks)) {

			// this returns only milliseconds (this is an error and should not happen after Win XP)
			uint64_t m_ticks = 0;
			m_ticks = (UINT64)timeGetTime() * 1000;
			return m_ticks;

		}

		// return microseconds
		ticks.QuadPart *= 1000000;
		ticks.QuadPart /= PCFreq;

		return ticks.QuadPart;
	}
}