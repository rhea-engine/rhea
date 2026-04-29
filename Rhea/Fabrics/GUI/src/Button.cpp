/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#include <Rhea/GUI/Button.h>
#include <Rhea/Application.h>
#include <Rhea/UnitTree.h>
#include <Psio/Log.h>
#include <Psio/Input/Keyboard.h>
#include <Rhea/EventInput.h>

namespace rhea
{

	void Button::setup(Application* app, std::unordered_map<std::string, std::string> attributes, std::unordered_map<std::string, Variant> values)
	{
		Widget::setup(app, attributes, values);

		std::unordered_map<std::string, std::string> attr;
		std::unordered_map<std::string, Variant> val;

		val["text"] = getValue("text", "");
		if (m_values.find("font") != m_values.end())
			val["font"] = getValue("font");

		// add a label
		m_label = (Label*)m_app->getUnitTree()->createUnit(m_path + "_label", "//meworla/gui/label", attr, val);
		addChild(m_label);

		if (!m_alignment)
			m_alignment = std::make_unique<Alignment>(Alignment::Horizontal::CENTER, Alignment::Vertical::CENTER);
	}

	void Button::setText(std::string text)
	{
		if (m_label)
			m_label->setText(text);
	}
	
	bool Button::onMousePressed(Event* ev, Unit* unit)
	{
		// consume the mouse event
		ev->stopPropagation();

		// send clicked event
		Event my_ev = Event("pressed", this);

		// send to local listeners
		sendEvent("pressed", &my_ev);

		if (m_values.find("onClick") != m_values.end()) {
			sendBoundEvent(m_values["onClick"].toString(), &my_ev);
		}

		return false;
	}

	bool Button::onMouseClicked(Event* ev, Unit* unit)
	{
		// consume the mouse event
		ev->stopPropagation();

		// send clicked event
		Event my_ev = Event("clicked", this);
		
		// always send to local listeners
		sendEvent("clicked", &my_ev);

		if (m_values.find("action") != m_values.end()) {
			bubbleEvent(m_values["action"].toString(), &my_ev);
		}

		if (m_values.find("onClicked") != m_values.end()) {
			sendBoundEvent(m_values["onClicked"].toString(), &my_ev);
		}

		return false;
	}

	bool Button::onKeyPressed(Event* ev, Unit* unit)
	{
		if (!hasFocus())
			return false;

		EventInput* inputEv = dynamic_cast<EventInput*>(ev);
		if (!inputEv)
			return false;
		InputEvent e = inputEv->getEvent();		

		if (e.key == Key::Enter || e.key == Key::Space) {
			onMouseClicked(ev, unit);
			return true;
		}

		return false;
	}

	void Button::onSetPosition()
	{
		// for now we assume only one child is possible
		// currently the label

		// Calculate the anchor position for the label
		// This is the top-left of the content area
		Vector2 label_anchor = m_position + m_content_position;
		
		if (m_alignment)
		{
			// Apply alignment to calculate offset within content area
			Vector2 labelOuterSize = m_label->getOuterSize();
			m_alignment->apply(m_content_size, labelOuterSize, label_anchor);
		}

		// setPosition expects an anchor position
		// The label's base_position (if any) will be added inside setPosition()
		m_label->setPosition(label_anchor);
	}
}