/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

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