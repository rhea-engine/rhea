#ifndef RHEA_GUI_LIST_H
#define RHEA_GUI_LIST_H

#include "Widget.h"
#include "Label.h"

namespace Rhea
{
	class ListWidget : public Widget
	{
		MECLASS(ListWidget, Widget)
	public:
		ListWidget(std::string path_name) :
			ListWidget(path_name, "//meworla/gui/list")
		{}
		ListWidget(std::string path_name, std::string type_name) :
			Widget(path_name, type_name)
		{}

		void setup(Application* app, std::unordered_map<std::string, std::string> attributes, std::unordered_map<std::string, Variant> values);

		void addItem(std::string text, std::string value);

		// list item selected
		bool onSelected(Event* ev, Unit* unit);

	protected:

	};
}

#endif // !RHEA_GUI_LIST_H
