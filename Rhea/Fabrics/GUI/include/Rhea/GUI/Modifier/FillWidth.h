#ifndef RHEA_GUI_MODIFIER_FILL_WIDTH_H
#define RHEA_GUI_MODIFIER_FILL_WIDTH_H

#include "Modifier.h"
#include "../Widget.h"

namespace rhea
{
	class FillWidthModifier : public Modifier
	{
	public:
		void apply(Widget* w, Vector2& size_constraints, Vector2& inner_size, Vector2& outer_size, Vector2& margin) {			
			// inner_size is set to parent width minus all size that is already set (padding)
			inner_size.x = size_constraints.x;
			size_constraints.x = inner_size.x;
		}

		std::string name() { return "fill_width"; }
	};
}
#endif // !RHEA_GUI_MODIFIER_FILL_WIDTH_H
