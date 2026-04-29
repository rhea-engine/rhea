/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

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

		std::string name() const override { return "fill_width"; }

		// equal operator for updating
		bool equals(const Modifier& other) const override {
			return name() == other.name();
		}
	};
}
#endif // !RHEA_GUI_MODIFIER_FILL_WIDTH_H
