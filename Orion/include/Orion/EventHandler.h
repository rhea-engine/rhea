/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#ifndef ORION_EVENT_HANDLER
#define ORION_EVENT_HANDLER

#include <Orion/Event.h>

namespace orion
{
	class EventHandler
	{
	public:
		virtual void onEvent(Event* event) = 0;		
	};
}

#endif // !ORION_EVENT_HANDLER
