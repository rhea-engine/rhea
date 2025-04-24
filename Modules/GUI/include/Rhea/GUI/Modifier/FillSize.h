#ifndef RHEA_GUI_MODIFIER_FILLSIZE_H
#define RHEA_GUI_MODIFIER_FILLSIZE_H

#include "Modifier.h"
#include "../Widget.h"

namespace Rhea
{
	class FillSizeModifier : public Modifier
	{
	public:
		void apply(Widget* w, Vector2& size_constraints, Vector2& inner_size, Vector2& outer_size, Vector2& margin) {
			// inner_size is set to parent size minus all size that is already set (padding)
			inner_size = size_constraints;
			size_constraints = inner_size;

		}

		std::string name() { return "fill_size"; }
	};
}

#endif // !RHEA_GUI_MODIFIER_FILLSIZE_H
