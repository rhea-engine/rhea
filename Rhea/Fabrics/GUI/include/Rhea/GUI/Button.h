#ifndef RHEA_GUI_BUTTON_H
#define RHEA_GUI_BUTTON_H

#include "Widget.h"
#include "Label.h"

namespace rhea
{
	class Button : public Widget
	{
		MECLASS(Button, Widget)
	public:
		Button(std::string path_name) :
			Button(path_name, "//meworla/gui/button")
		{}
		Button(std::string path_name, std::string type_name) :
			Widget(path_name, type_name),
			m_label(nullptr)
		{}

		void setup(Application* app, std::unordered_map<std::string, std::string> attributes, std::unordered_map<std::string, Variant> values);

		void setText(std::string text);

		// input handling
		bool onMouseClick(Event* ev, Unit* unit) override;
		bool onMouseClicked(Event* ev, Unit* unit) override;

		// sizing
		void onSetPosition();

		// styling
		const std::string getSelector() { return "Button"; }

	protected:
		Label* m_label;
	};
}

#endif // !RHEA_GUI_BUTTON_H
