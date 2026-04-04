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
