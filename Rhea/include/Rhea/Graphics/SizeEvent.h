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
