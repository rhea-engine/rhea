#ifndef PSIO_TIMER_H
#define PSIO_TIMER_H

#include <cstdint>

namespace Psio
{
	class Timer
	{
	public:
		static uint64_t getTicks();
	};
}
#endif