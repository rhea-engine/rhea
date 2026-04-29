/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#ifndef RHEA_GUI_SUBMITEVENT_H
#define RHEA_GUI_SUBMITEVENT_H

#include <unordered_map>

#include "../WidgetEvent.h"

namespace rhea
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
