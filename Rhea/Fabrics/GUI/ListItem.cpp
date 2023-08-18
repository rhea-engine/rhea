#include "ListItem.h"
#include <Rhea/Application.h>
#include <Rhea/UnitTree.h>

namespace Rhea 
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

		m_handle_input = true;

		// add a label
		m_label = (Label*)m_app->getUnitTree()->createUnit(m_path + "/label", "//meworla/gui/label", attr, val);
		addChild(m_label);

		registerEventHandler("mouse_btn_down", this, std::bind(&ListWidgetItem::onMouseClick, this, std::placeholders::_1, std::placeholders::_2));
	}
	
	bool ListWidgetItem::onMouseClick(Event* ev, Unit* unit)
	{
		// consume the mouse event
		ev->stopPropagation();

		// send selected event
		Event my_ev = Event("selected", this);
		bubbleEvent("selected", &my_ev);		
		return false;
	}
}
