#ifndef RHEA_GUI_COLUMN_H
#define RHEA_GUI_COLUMN_H

#include "Widget.h"

namespace Rhea {

	class ColumnWidget : public Widget
	{
		MECLASS(ColumnWidget, Widget)

	public:
		ColumnWidget(std::string name, std::string type_name) :		
			m_full_weight(0.0f),
			m_children_size(0.0f),
			Widget(name, type_name)
		{}

		void ready();

		// recompute widget size
		void onSetPosition();

		Vector2 innerSize(Vector2 size_constraints);

		void innerGetMinSize(Vector2 size_constraints, Vector2& min_size);

	protected:
		float m_full_weight;
		float m_children_size;
	};
}

#endif // !RHEA_GUI_COLUMN_H
