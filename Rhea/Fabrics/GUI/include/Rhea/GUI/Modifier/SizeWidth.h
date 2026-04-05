#ifndef RHEA_GUI_MODIFIER_SIZEWIDTH_H
#define RHEA_GUI_MODIFIER_SIZEWIDTH_H

#include "Modifier.h"
#include "../Widget.h"
#include <Math/Vector2.h>

namespace rhea {

	class SizeWidthModifier : public Modifier
	{
	public:
		SizeWidthModifier(float size) :
			m_size(size)
		{}

		void apply(Widget* w, Vector2& size_constraints, Vector2& inner_size, Vector2& outer_size, Vector2& margin) {
			inner_size.x = m_size;
			if (size_constraints.x < inner_size.x)
				inner_size.x = size_constraints.x;
			size_constraints.x = m_size;
		};

		void essentialSize(Widget* w, Vector2& min_inner_size, Vector2& max_inner_size, Vector2& min_outer_size) {
			min_inner_size.x = m_size;
			if (max_inner_size.x < min_inner_size.x)
				min_inner_size.x = max_inner_size.x;
			max_inner_size.x = m_size;
		}

		std::string name() { return "width"; }

	protected:
		float m_size;
	};
}

#endif // !RHEA_GUI_MODIFIER_SIZEWIDTH_H
