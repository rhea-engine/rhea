#ifndef RHEA_GUI_LIST_H
#define RHEA_GUI_LIST_H

#include "Widget.h"
#include "Label.h"
#include <Rhea/UnitTree.h>

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
			m_data_binding({nullptr, std::string("")}),
			Widget(path_name, type_name)
		{}

		void setup(Application* app, std::unordered_map<std::string, std::string> attributes, std::unordered_map<std::string, Variant> values);

		void addItem(std::string text, std::string value);

		void ready();

		// frame handler
		void onFrame(float delta);
		void pollData();

		// list item selected
		bool onSelected(Event* ev, Unit* unit);
		
		// move the current position
		void onSetPosition();

		// recalculate my size
		Vector2 innerSize(Vector2 size_constraints);

	protected:
		UnitTree::Binding m_data_binding;
	};
}

#endif // !RHEA_GUI_LIST_H
