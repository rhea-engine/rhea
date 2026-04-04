#ifndef RHEA_GUI_MODIFIER_SIZE_H
#define RHEA_GUI_MODIFIER_SIZE_H

#include "Modifier.h"
#include "../Widget.h"
#include <Math/Vector2.h>

namespace rhea {

	class SizeModifier : public Modifier
	{
	public:
		SizeModifier(math::Vector2 size) :
			m_size(size)
		{}

		void apply(Widget* w, Vector2& size_constraints, Vector2& inner_size, Vector2& outer_size, Vector2& margin) {
			inner_size = m_size; 
			// clamp to constraint
			if (size_constraints.x < inner_size.x)
				inner_size.x = size_constraints.x;
			if (size_constraints.y < inner_size.y)
				inner_size.y = size_constraints.y;
			size_constraints = m_size;
		};

		void essentialSize(Widget* w, Vector2& min_inner_size, Vector2& max_inner_size, Vector2& min_outer_size) {
			min_inner_size = m_size; 
			if (max_inner_size.x < min_inner_size.x)
				min_inner_size.x = max_inner_size.x;
			if (max_inner_size.y < min_inner_size.y)
				min_inner_size.y = max_inner_size.y;			
			max_inner_size = m_size;
		}

		std::string name() { return "size"; }

	protected:
		math::Vector2 m_size;
	};
}

#endif // !RHEA_GUI_MODIFIER_SIZE_H
