#ifndef RHEA_GUI_MODIFIER_FILL_HEIGHT_H
#define RHEA_GUI_MODIFIER_FILL_HEIGHT_H

#include "Modifier.h"
#include "../Widget.h"

namespace rhea
{
	class FillHeightModifier : public Modifier
	{
	public:
		void apply(Widget* w, Vector2& size_constraints, Vector2& inner_size, Vector2& outer_size, Vector2& margin) {
			// inner_size is set to parent height minus all size that is already set (padding)
			inner_size.y = size_constraints.y;
			size_constraints.y = inner_size.y;
		}

		std::string name() const override { return "fill_height"; }

		// equals is used to check if the modifier is the same as another for updating
		bool equals(const Modifier& other) const override {
			return name() == other.name();
		}
	};
}
#endif // !RHEA_GUI_MODIFIER_FILL_HEIGHT_H
