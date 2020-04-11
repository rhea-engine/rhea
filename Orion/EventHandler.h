#ifndef ORION_EVENT_HANDLER
#define ORION_EVENT_HANDLER

#include "Event.h"

namespace Orion
{
	class EventHandler
	{
	public:
		virtual void onEvent(Event* event) = 0;		
	};
}

#endif // !ORION_EVENT_HANDLER
