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

		val["text"] = getValue("text", "");
		if (m_values.find("font") != m_values.end())
			val["font"] = getValue("font");

		m_handle_input = true;

		// add a label
		m_label = (Label*)m_app->getUnitTree()->createUnit(m_path + "/label", "//meworla/gui/label", attr, val);
		addChild(m_label);

		if (!m_alignment)
			m_alignment = std::make_unique<Alignment>(Alignment::Horizontal::CENTER, Alignment::Vertical::CENTER);

		registerEventHandler("mouse_btn_down", this, std::bind(&Button::onMouseClick, this, std::placeholders::_1, std::placeholders::_2));
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

	void Button::onSetPosition()
	{
		// for now we assume only one child is possible
		// currently the label

		Vector2 child_pos = m_position;
		if (m_alignment)
			m_alignment->apply(m_size, m_label->getOuterSize(), child_pos);

		m_label->setPosition(child_pos);

	}
}