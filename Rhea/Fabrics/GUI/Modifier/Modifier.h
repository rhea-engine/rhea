#ifndef RHEA_GUI_MODIFIER_H
#define RHEA_GUI_MODIFIER_H

#include <Math/Vector2.h>
#include <string>

using Math::Vector2;

namespace Rhea
{
	class Widget;

	class Modifier
	{
	public:
		// apply the modifier to the Widget w
		virtual void apply(Widget *w, Vector2& size_constraints, Vector2& inner_size, Vector2& outer_size) = 0;

		// get the minimal size
		virtual void essentialSize(Widget* w, Vector2& min_inner_size, Vector2& max_inner_size, Vector2& min_outer_size) { };

		// return a name of this modifier 
		// this allows the updating of the modifier
		virtual std::string name() = 0;
	};
}

#endif // !RHEA_GUI_MODIFIER_H
