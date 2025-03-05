#ifndef RHEA_GUI_ROW_H
#define RHEA_GUI_ROW_H

#include "Widget.h"

namespace Rhea {

	class RowWidget : public Widget
	{
		MECLASS(RowWidget, Widget)

	public:
		RowWidget(std::string name, std::string type_name) :
			m_children_size(0),
			Widget(name, type_name)
		{
			m_justify_mode = 1;
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
