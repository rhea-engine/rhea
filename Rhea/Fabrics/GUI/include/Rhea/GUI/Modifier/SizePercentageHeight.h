#ifndef RHEA_GUI_MODIFIER_SIZE_PERCENTAGE_HEIGHT_H
#define RHEA_GUI_MODIFIER_SIZE_PERZENTAGE_HEIGHT_H

#include "Modifier.h"
#include "../Widget.h"
#include <Math/Vector2.h>

namespace rhea {

	class SizePercentageHeightModifier : public Modifier
	{
	public:
		SizePercentageHeightModifier(float size) :
			m_size(size)
		{}

		void apply(Widget* w, Vector2& size_constraints, Vector2& inner_size, Vector2& outer_size, Vector2& margin) {
			Vector2 p_size;
			if (doCast<Widget>(w->getParent()))
				p_size = doCast<Widget>(w->getParent())->getSize();
			else
				p_size = size_constraints;

			inner_size.y = m_size * p_size.y;

			// clamp to constraint
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
			min_inner_size.y = m_size * p_size.y;
			if (max_inner_size.y < min_inner_size.y)
				min_inner_size.y = max_inner_size.y;
			max_inner_size = min_inner_size;
		}

		std::string name() const override { return "size_percentage_height"; }

		bool equals(const Modifier& other) const override {
			if (other.name() != name())
				return false;
			const SizePercentageHeightModifier& o = static_cast<const SizePercentageHeightModifier&>(other);
			return m_size == o.m_size;
		}

	protected:
		float m_size;
	};
}

#endif // !RHEA_GUI_MODIFIER_SIZE_PERZENTAGE_HEIGHT_H
