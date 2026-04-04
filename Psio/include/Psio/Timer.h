#ifndef PSIO_TIMER_H
#define PSIO_TIMER_H

#include <cstdint>

namespace psio
{
	class Timer
	{
	public:
		static uint64_t getTicks();
	};
}
#endif