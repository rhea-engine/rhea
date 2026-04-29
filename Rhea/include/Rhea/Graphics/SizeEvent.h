/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#ifndef RHEA_SIZE_EVENT_H
#define RHEA_SIZE_EVENT_H

#include "../Event.h"

namespace rhea
{
	class SizeEvent :
		public Event
	{
	public:
		SizeEvent(Unit* target, int width, int height) :
			Event("size", target),
			width(width),
			height(height)
		{
		}
		int width;
		int height;
	};
}

#endif // !RHEA_SIZE_EVENT_H
