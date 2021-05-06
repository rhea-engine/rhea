#ifndef RHEA_GUI_WIDGETEVENT_H
#define RHEA_GUI_WIDGETEVENT_H

#include "../../Event.h"

namespace Rhea
{
	class Widget;

	class WidgetEvent : public Event
	{
	public:
		WidgetEvent(std::string name, Widget* w) :
			Event(name),
			m_w(w)
		{}

		Widget* getWidget() { return m_w; }

	protected:
		Widget* m_w;
	};
}

#endif // !RHEA_GUI_WIDGETEVENT_H
