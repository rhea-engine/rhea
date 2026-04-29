/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#ifndef RHEA_GUI_MODIFIER_SIZE_PERCENTAGE_H
#define RHEA_GUI_MODIFIER_SIZE_PERZENTAGE_H

#include "Modifier.h"
#include "../Widget.h"
#include <Math/Vector2.h>

namespace rhea {

	class SizePercentageModifier : public Modifier
	{
	public:
		SizePercentageModifier(math::Vector2 size) :
			m_size(size)
		{}

		void apply(Widget* w, Vector2& size_constraints, Vector2& inner_size, Vector2& outer_size, Vector2& margin) {
			Vector2 p_size;
			if (doCast<Widget>(w->getParent()))
				p_size = doCast<Widget>(w->getParent())->getContainingBlock();
			else
				p_size = size_constraints;
			
			inner_size.x = m_size.x * p_size.x;
			inner_size.y = m_size.y * p_size.y;
			// clamp to constraint
			if (size_constraints.x < inner_size.x)
				inner_size.x = size_constraints.x;
			if (size_constraints.y < inner_size.y)
				inner_size.y = size_constraints.y;
			size_constraints = inner_size;
		};

		void essentialSize(Widget* w, Vector2& min_inner_size, Vector2& max_inner_size, Vector2& min_outer_size) {
			Vector2 p_size;
			if (doCast<Widget>(w->getParent()))
				p_size = doCast<Widget>(w->getParent())->getSize();
			else
				p_size = min_inner_size;
			
			min_inner_size.x = m_size.x * p_size.x;
			min_inner_size.y = m_size.y * p_size.y;
			if (max_inner_size.x < min_inner_size.x)
				min_inner_size.x = max_inner_size.x;
			if (max_inner_size.y < min_inner_size.y)
				min_inner_size.y = max_inner_size.y;
			max_inner_size = min_inner_size;
		}

		std::string name() const override { return "size_percentage"; }

		bool equals(const Modifier& other) const override {
			if (other.name() != name())
				return false;
			const SizePercentageModifier& o = static_cast<const SizePercentageModifier&>(other);
			return m_size.equals(o.m_size);
		}

	protected:
		math::Vector2 m_size;
	};
}

#endif // !RHEA_GUI_MODIFIER_SIZE_PERZENTAGE_H
