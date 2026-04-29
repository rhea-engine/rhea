/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#ifndef RHEA_GUI_LISTITEM_H
#define RHEA_GUI_LISTITEM_H

#include "Widget.h"
#include "Label.h"

namespace rhea
{
	class ListWidgetItem : public Widget
	{
		MECLASS(ListWidgetItem, Widget)
	public:
		ListWidgetItem(std::string path_name) :
			ListWidgetItem(path_name, "//meworla/gui/listitem")
		{}
		ListWidgetItem(std::string path_name, std::string type_name) :
			m_label(nullptr), 
			Widget(path_name, type_name)
		{}

		void setup(Application* app, std::unordered_map<std::string, std::string> attributes, std::unordered_map<std::string, Variant> values);

		void ready();

		// input handling
		bool onMousePressed(Event* ev, Unit* unit) override;
		bool onMouseClicked(Event* ev, Unit* unit) override;
		bool onKeyPressed(Event* ev, Unit* unit) override;

		// styling
		const std::string getSelector() { return "ListItem"; }

	protected:
		Label* m_label;
	};
}

#endif // !RHEA_GUI_LISTITEM_H
