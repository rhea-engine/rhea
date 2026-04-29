/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#include <Psio/Timer.h>

#include <time.h>
#include <cstdint>

namespace psio
{
	uint64_t Timer::getTicks()
	{
		struct timespec ts;
		
		// Try CLOCK_MONOTONIC for high-resolution monotonic time
		if (clock_gettime(CLOCK_MONOTONIC, &ts) == 0) {
			// Convert to microseconds
			uint64_t ticks = static_cast<uint64_t>(ts.tv_sec) * 1000000ULL;
			ticks += static_cast<uint64_t>(ts.tv_nsec) / 1000ULL;
			return ticks;
		}
		
		// Fallback to CLOCK_REALTIME if CLOCK_MONOTONIC fails
		if (clock_gettime(CLOCK_REALTIME, &ts) == 0) {
			uint64_t ticks = static_cast<uint64_t>(ts.tv_sec) * 1000000ULL;
			ticks += static_cast<uint64_t>(ts.tv_nsec) / 1000ULL;
			return ticks;
		}
		
		// This should never happen on modern Unix systems
		return 0;
	}
}