/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#ifndef RHEA_GUI_WIDGETEVENT_H
#define RHEA_GUI_WIDGETEVENT_H

#include <Rhea/Event.h>

namespace rhea
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
