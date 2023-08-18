#ifndef RHEA_GUI_MODIFIER_OFFSET_H
#define RHEA_GUI_MODIFIER_OFFSET_H

#include "Modifier.h"
#include "../Widget.h"
#include <Math/Vector2.h>

namespace Rhea {

	class OffsetModifier : public Modifier
	{
	public:
		OffsetModifier(Math::Vector2 offset) :
			m_offset(offset)
		{}

		void apply(Widget* w, Vector2& size_constraints, Vector2& inner_size, Vector2& outer_size) {
			w->setBasePosition(m_offset + w->getBasePosition());
		};

		std::string name() { return "offset"; }

	protected:
		Math::Vector2 m_offset;
	};
}

#endif // !RHEA_GUI_MODIFIER_OFFSET_H
