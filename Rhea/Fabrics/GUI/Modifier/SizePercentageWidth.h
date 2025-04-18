#ifndef RHEA_GUI_MODIFIER_SIZE_PERCENTAGE_WIDTH_H
#define RHEA_GUI_MODIFIER_SIZE_PERZENTAGE_WIDTH_H

#include "Modifier.h"
#include "../Widget.h"
#include <Math/Vector2.h>

namespace Rhea {

	class SizePercentageWidthModifier : public Modifier
	{
	public:
		SizePercentageWidthModifier(float size) :
			m_size(size)
		{}

		void apply(Widget* w, Vector2& size_constraints, Vector2& inner_size, Vector2& outer_size, Vector2& margin) {
			Vector2 p_size;
			if (doCast<Widget>(w->getParent()))
				p_size = doCast<Widget>(w->getParent())->getSize();
			else
				p_size = size_constraints;

			inner_size.x = m_size * p_size.x;
			
			// clamp to constraint
			if (size_constraints.x < inner_size.x)
				inner_size.x = size_constraints.x;
			outer_size += inner_size;
		};

		void essentialSize(Widget* w, Vector2& min_inner_size, Vector2& max_inner_size, Vector2& min_outer_size) {
			Vector2 p_size;
			if (doCast<Widget>(w->getParent()))
				p_size = doCast<Widget>(w->getParent())->getSize();
			else
				p_size = min_inner_size;
			min_inner_size.x = m_size * p_size.x;
			if (max_inner_size.x < min_inner_size.x)
				min_inner_size.x = max_inner_size.x;
			max_inner_size = min_inner_size;
		}

		std::string name() { return "size_percentage_width"; }

	protected:
		float m_size;
	};
}

#endif // !RHEA_GUI_MODIFIER_SIZE_PERZENTAGE_WIDTH_H
