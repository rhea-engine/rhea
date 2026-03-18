#ifndef RHEA_GUI_WIDGETEVENT_H
#define RHEA_GUI_WIDGETEVENT_H

#include <Rhea/Event.h>

namespace Rhea
{
	class Widget;

	class WidgetEvent : public Event
	{
	public:
		WidgetEvent(std::string name, Widget* w) :
			Event(name, (Unit*)w)			
		{}

		Widget* getWidget() { return (Widget*)m_target; }
	};
}

#endif // !RHEA_GUI_WIDGETEVENT_H
