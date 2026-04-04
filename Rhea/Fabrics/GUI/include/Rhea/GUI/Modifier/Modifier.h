#ifndef RHEA_GUI_MODIFIER_H
#define RHEA_GUI_MODIFIER_H

#include <Math/Vector2.h>
#include <string>

using Math::Vector2;

namespace rhea
{
	class Widget;

	class Modifier
	{
	public:
		// apply the modifier to the Widget w
		// The apply() method updates:
		// - size_constraints is the starting available space for content updated to the available space for content after this modifier is applied.
		// - inner_size: the minmal needed space for the box after this modifier is applied.
		// - outer_size: any extra space (e.g., padding) that must be added to the content.		
		// - margin: any extra margin outside of inner_size
		virtual void apply(Widget *w, Vector2& size_constraints, Vector2& inner_size, Vector2& outer_size, Vector2& margin) = 0;

		// get the minimal size
		virtual void essentialSize(Widget* w, Vector2& min_inner_size, Vector2& max_inner_size, Vector2& min_outer_size) { };

		// return a name of this modifier 
		// this allows the updating of the modifier
		virtual std::string name() = 0;
	};
}

#endif // !RHEA_GUI_MODIFIER_H
