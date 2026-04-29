/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#include <Rhea/GUI/ListItem.h>
#include <Rhea/Application.h>
#include <Rhea/UnitTree.h>
#include <Psio/Input/Keyboard.h>
#include <Rhea/EventInput.h>

namespace rhea 
{
	void ListWidgetItem::setup(Application* app, std::unordered_map<std::string, std::string> attributes, std::unordered_map<std::string, Variant> values)
	{
		Widget::setup(app, attributes, values);

		std::unordered_map<std::string, std::string> attr;
		std::unordered_map<std::string, Variant> val;

		val["padding"] = getValue("padding", 5.0f);
		val["text"] = getValue("text", "");
		val["fillwidth"] = true;
		if (m_values.find("font") != m_values.end())
			val["font"] = getValue("font");

		// add a label
		m_label = (Label*)m_app->getUnitTree()->createUnit(m_path + "/label", "//meworla/gui/label", attr, val);
		addChild(m_label);
	}

	void ListWidgetItem::ready()
	{
		Widget::ready();

		if (m_label)
			m_label->ready();
	}
	
	bool ListWidgetItem::onMousePressed(Event* ev, Unit* unit)
	{
		// consume the mouse event
		ev->stopPropagation();

		// send selected event
		Event my_ev = Event("selected", this);
		bubbleEvent("selected", &my_ev);		
		return false;
	}

	bool ListWidgetItem::onMouseClicked(Event* ev, Unit* unit)
	{
		// consume the mouse event
		ev->stopPropagation();
		// send clicked event
		Event my_ev = Event("clicked", this);
		bubbleEvent("clicked", &my_ev);
		return false;
	}

	bool ListWidgetItem::onKeyPressed(Event* ev, Unit* unit)
	{
		EventInput* inputEv = dynamic_cast<EventInput*>(ev);
		if (!inputEv)
			return false;
		InputEvent e = inputEv->getEvent();

		if (e.key == Key::Enter || e.key == Key::Space) {
			onMouseReleased(ev, unit);
			return true;
		}

		return false;
	}
}
