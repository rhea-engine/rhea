/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#ifndef RHEA_GUI_MODIFIER_SIZEHEIGHT_H
#define RHEA_GUI_MODIFIER_SIZEHEIGHT_H

#include "Modifier.h"
#include "../Widget.h"
#include <Math/Vector2.h>

namespace rhea {

	class SizeHeightModifier : public Modifier
	{
	public:
		SizeHeightModifier(float size) :
			m_size(size)
		{}

		void apply(Widget* w, Vector2& size_constraints, Vector2& inner_size, Vector2& outer_size, Vector2& margin) {
			inner_size.y = m_size;
			if (size_constraints.y < inner_size.y)
				inner_size.y = size_constraints.y;
			size_constraints.y = m_size;
		};

		void essentialSize(Widget* w, Vector2& min_inner_size, Vector2& max_inner_size, Vector2& min_outer_size) {
			min_inner_size.y = m_size;			
			if (max_inner_size.y < min_inner_size.y)
				min_inner_size.y = max_inner_size.y;
			max_inner_size.y = m_size;
		}

		std::string name() const override { return "height"; }

		bool equals(const Modifier& other) const override {
			if (name() != other.name())
				return false;
			const SizeHeightModifier& size_other = static_cast<const SizeHeightModifier&>(other);
			return m_size == size_other.m_size;
		}

	protected:
		float m_size;
	};
}

#endif // !RHEA_GUI_MODIFIER_SIZEHEIGHT_H
