#ifndef ORION_IOCOMPLETEHANDLER_H
#define ORION_IOCOMPLETEHANDLER_H

#include "Event.h"
#include "EventHandler.h"

namespace Orion
{
	class IOCompleteHandler : public EventHandler
	{
	public:
		virtual void onIOComplete(Event* e) = 0;
	};
}

#endif // !ORION_IOCOMPLETEHANDLER_H
