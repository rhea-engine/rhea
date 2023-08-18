#ifndef RHEA_GUI_BOX_H
#define RHEA_GUI_BOX_H

#include "Widget.h"

namespace Rhea {

	class BoxWidget : public Widget
	{
		MECLASS(BoxWidget, Widget)

	public:
		BoxWidget(std::string name, std::string type_name) :
			Widget(name,type_name)
		{}

		void ready();

		// recompute widget size
		void onSetPosition();

	protected:

	};
}

#endif // !RHEA_GUI_BOX_H
