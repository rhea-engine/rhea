#include "Button.h"
#include <Rhea/Application.h>
#include <Rhea/UnitTree.h>
#include <Psio/Log.h>

namespace Rhea
{

	void Button::setup(Application* app, std::unordered_map<std::string, std::string> attributes, std::unordered_map<std::string, Variant> values)
	{
		Widget::setup(app, attributes, values);

		std::unordered_map<std::string, std::string> attr;
		std::unordered_map<std::string, Variant> val;

		val["x"] = 5;
		val["y"] = 5;
		val["w"] = m_base_size.x-5;
		val["h"] = m_base_size.y-5;
		val["text"] = getValue("text", "");
		if (m_values.find("font") != m_values.end())
			val["font"] = getValue("font");

		m_handle_input = true;

		// add a label
		m_label = (Label*)m_app->getUnitTree()->createUnit(m_path + "/label", "//meworla/gui/label", attr, val);
		addChild(m_label);

		registerEventHandler("mouse_btn_down", this, std::bind(&Button::onMouseClick, this, std::placeholders::_1, std::placeholders::_2));
	}

	Button* Button::createType(std::string name, std::string type_name)
	{
		Button* ret = new Button(name, type_name);
		return ret;
	}

	void Button::setText(std::string text)
	{
		if (m_label)
			m_label->setText(text);
	}
	
	bool Button::onMouseClick(Event* ev, Unit* unit)
	{
		// consume the mouse event
		ev->stopPropagation();

		// send clicked event
		Event my_ev = Event("clicked", this);
		bubbleEvent("clicked", &my_ev);
		if (m_values["onClick"] != Variant()) {
			sendBoundEvent(m_values["onClick"], ev);
		}
		return false;
	}
}