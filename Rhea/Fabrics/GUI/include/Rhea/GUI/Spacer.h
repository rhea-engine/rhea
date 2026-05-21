/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#ifndef RHEA_GUI_SPACER_H
#define RHEA_GUI_SPACER_H

#include "Widget.h"

namespace rhea {

	class SpacerWidget : public Widget
	{
		MECLASS(SpacerWidget, Widget)

	public:
		SpacerWidget(std::string name, std::string type_name) :
			Widget(name, type_name)
		{
			m_is_renderable = false;
		}

		// styling
		const std::string getSelector() { return "Spacer"; }
	};
}

#endif // !RHEA_GUI_SPACER_H
