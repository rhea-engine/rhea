/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#ifndef RHEA_GUI_COLUMN_H
#define RHEA_GUI_COLUMN_H

#include "Widget.h"

namespace rhea {

	class ColumnWidget : public Widget
	{
		MECLASS(ColumnWidget, Widget)

	public:
		ColumnWidget(std::string name, std::string type_name) :		
			m_full_weight(0.0f),
			m_children_size(0.0f),
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
		const std::string getSelector() { return "Column"; }

	protected:
		float m_full_weight;
		float m_children_size;
	};
}

#endif // !RHEA_GUI_COLUMN_H
