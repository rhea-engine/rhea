#ifndef RHEA_GUI_MODIFIER_PADDING_H
#define RHEA_GUI_MODIFIER_PADDING_H

#include "Modifier.h"
#include "../Widget.h"

namespace Rhea
{
	class PaddingModifier : public Modifier
	{
	public:
		PaddingModifier(float padding) :			
			m_topleft(padding, padding),
			m_rightbottom(padding, padding)
		{}
		PaddingModifier(float padding_left, float padding_top, float padding_right, float padding_bottom) :
			m_topleft(padding_left, padding_top),
			m_rightbottom(padding_right, padding_bottom)
		{}
		void apply(Widget* w, Vector2& size_constraints, Vector2& inner_size, Vector2& outer_size, Vector2& margin) {
			
			// horizontal padding
			size_constraints.x -= (m_topleft.x + m_rightbottom.x);
			outer_size.x += m_topleft.x + m_rightbottom.x;
			
			// vertical padding			
			size_constraints.y -= (m_topleft.y + m_rightbottom.y);
			outer_size.y += m_topleft.y + m_rightbottom.y;

			// adjust position
			Vector2 pos = w->getContentPosition();
			pos.x += m_topleft.x;
			pos.y += m_topleft.y;
			w->setContentPosition(pos);

			// if constraints become negative set them to zero
			if (size_constraints.x < 0.0f) {
				outer_size.x -= size_constraints.x;
				size_constraints.x = 0.0f;
			}
			if (size_constraints.y < 0.0f) {
				outer_size.y -= size_constraints.y;
				size_constraints.y = 0.0f;
			}
		}

		void essentialSize(Widget* w, Vector2& min_inner_size, Vector2& max_inner_size, Vector2& min_outer_size) {
			max_inner_size -= (m_topleft + m_rightbottom);
			min_outer_size += m_topleft + m_rightbottom;
		}

		std::string name() { return "padding"; }
	protected:		
		Vector2 m_topleft; 
		Vector2 m_rightbottom;
	};
}

#endif // !RHEA_GUI_MODIFIER_PADDING_H
