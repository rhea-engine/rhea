/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#ifndef RHEA_GUI_BOX_H
#define RHEA_GUI_BOX_H

#include "Widget.h"

namespace rhea {

	class BoxWidget : public Widget
	{
		MECLASS(BoxWidget, Widget)

	public:
		BoxWidget(std::string name, std::string type_name) :
			Widget(name,type_name)
		{
			m_is_renderable = false;
		}

		void ready();

		// recompute widget size
		void onSetPosition();

		// styling
		const std::string getSelector() { return "Box"; }

	protected:

	};
}

#endif // !RHEA_GUI_BOX_H
