#ifndef RHEA_GUI_SPACER_H
#define RHEA_GUI_SPACER_H

#include "Widget.h"

namespace Rhea {

	class SpacerWidget : public Widget
	{
		MECLASS(SpacerWidget, Widget)

	public:
		SpacerWidget(std::string name, std::string type_name) :
			Widget(name, type_name)
		{
			m_justify_mode = 1;
			m_is_renderable = false;
		}

		// styling
		const std::string getSelector() { return "Spacer"; }
	};
}

#endif // !RHEA_GUI_SPACER_H
