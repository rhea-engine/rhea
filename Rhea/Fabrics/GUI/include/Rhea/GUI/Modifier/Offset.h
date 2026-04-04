#ifndef RHEA_GUI_MODIFIER_OFFSET_H
#define RHEA_GUI_MODIFIER_OFFSET_H

#include "Modifier.h"
#include "../Widget.h"
#include <Math/Vector2.h>

namespace rhea {

	class OffsetModifier : public Modifier
	{
	public:
		OffsetModifier(math::Vector2 offset) :
			m_offset(offset)
		{}

		void apply(Widget* w, Vector2& size_constraints, Vector2& inner_size, Vector2& outer_size, Vector2& margin) {
			w->setBasePosition(m_offset + w->getBasePosition());
		};

		std::string name() { return "offset"; }

	protected:
		math::Vector2 m_offset;
	};
}

#endif // !RHEA_GUI_MODIFIER_OFFSET_H
