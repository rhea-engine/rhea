/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#ifndef RHEA_GUI_ROW_H
#define RHEA_GUI_ROW_H

#include "Widget.h"

namespace rhea {

	class RowWidget : public Widget
	{
		MECLASS(RowWidget, Widget)

	public:
		RowWidget(std::string name, std::string type_name) :
			m_children_size(0),
			Widget(name, type_name)
		{
			m_justify_mode = 1;
			m_is_renderable = false;
		}

		void ready();

		// recompute widget size
		void onSetPosition();

		Vector2 innerSize(Vector2 size_constraints);

		void innerGetMinSize(Vector2 size_constraints, Vector2& min_size);

		// styling
		const std::string getSelector() { return "Row"; }

	protected:
		float m_children_size;
	};
}

#endif // !RHEA_GUI_ROW_H
