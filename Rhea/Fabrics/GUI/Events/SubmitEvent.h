#ifndef RHEA_GUI_SUBMITEVENT_H
#define RHEA_GUI_SUBMITEVENT_H

#include <unordered_map>

#include "../WidgetEvent.h"

namespace Rhea
{
	class SubmitEvent : public WidgetEvent
	{
	public:
		SubmitEvent(std::string name, Widget* w) :
			WidgetEvent(name, w)
		{}

		std::unordered_map<std::string, std::string> values;
	};
}

#endif // !RHEA_GUI_SUBMITEVENT_H
