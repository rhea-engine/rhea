/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#include <Rhea/GUI/TextInput.h>
#include <Rhea/Application.h>
#include <Rhea/UnitTree.h>
#include <Rhea/Core/Scene.h>
#include <Rhea/Text/TextWidget.h>
#include <Rhea/GUI/Label.h>
#include <Psio/Log.h>
#include <Psio/Input/Keyboard.h>
#include <Rhea/EventInput.h>
#include <Rhea/GUI/Events/SubmitEvent.h>

namespace rhea
{
	void TextInput::setup(Application* app, std::unordered_map<std::string, std::string> attributes, std::unordered_map<std::string, Variant> values)
	{
		Widget::setup(app, attributes, values);

		std::unordered_map<std::string, std::string> attr;
		std::unordered_map<std::string, Variant> val;

		val["text"] = getValue("text", "");
		val["templating"] = false; // dont template the user input
		m_text = val["text"].toString();
		m_values["text"] = m_text;
		val["placeholder"] = getValue("placeholder", "");
		// multiline is not implemented yet
		val["multiline"] = false;
		val["word_wrap"] = false;

		val["padding"] = 2.0f;

		if (!val["multiline"])
			val["fillwidth"] = true;
		else
			val["fillsize"] = true;
		m_placeholder = val["placeholder"].toString();
		m_pos = m_text.size();
		if (!m_pos) {
			val["text"] = m_placeholder;
		}
		m_length = m_pos;
		if (m_values.find("font") != m_values.end())
			val["font"] = getValue("font");

		m_label_item = (Label*)m_app->getUnitTree()->createUnit(getPathName() + "_label", "//meworla/gui/label", attr, val);
		addChild(m_label_item);
		// store reference to textwidget
		m_textwidget = m_label_item->getTextWidget();

		// offset of "|" assumed to be 2/2
		std::unordered_map<std::string, Variant> cursor_val;
		cursor_val["x"] = -2.0f;
		cursor_val["y"] = -2.0f;
		cursor_val["w"] = 20;
		cursor_val["h"] = 20;
		m_cursor_item = (Label*)m_app->getUnitTree()->createUnit(getPathName() + "_cursor", "//meworla/gui/label", attr, cursor_val);
		addChild(m_cursor_item);
		m_cursor_item->setText("|");
		m_cursor_item->hide();
	}

	void TextInput::setText(std::string text)
	{
		m_text = text;		
		m_pos = m_text.size();
		m_length = m_pos;
		updateText(false);
	}

	void TextInput::onFrame(float delta)
	{
		if (!m_has_focus)
			return;

		m_cursor_timer += delta;
		if (m_cursor_timer >= 0.7f) {
			m_cursor_shown = !m_cursor_shown;
			m_cursor_timer = 0.0f;
			(m_cursor_shown) ? m_cursor_item->makeVisible() : m_cursor_item->makeInvisible();
		}
	}

	Vector2 TextInput::innerSize(Vector2 size_constraints)
	{
		// size my label item
		Vector2 label_size = m_label_item->size(size_constraints);
		
		m_cursor_item->size(size_constraints);	

		return label_size;
	}

	void TextInput::onSetPosition()
	{
		m_label_item->setPosition(m_position + m_content_position);		
		float offset_x = m_textwidget->W() - m_textwidget->getTextWidth(m_pos);
		offset_x = (offset_x < 0.0f) ? offset_x : 0.0f;
		if (offset_x != m_offset_x) {
			m_textwidget->setOffset(offset_x);
			m_offset_x = offset_x;
			// mark for redraw
			m_textwidget->markTextDirty();
		}
		m_cursor_item->setPosition(m_cursor_position);
	}

	void TextInput::updateCursor(bool onlyCursor)
	{
		if (!m_textwidget)
			return;

		if (onlyCursor) {
			float offset_x = m_textwidget->W() - m_textwidget->getTextWidth(m_pos);
			offset_x = (offset_x < 0.0f) ? offset_x : 0.0f;
			if (offset_x != m_offset_x) {
				m_textwidget->setOffset(offset_x);
				m_offset_x = offset_x;
				// force redraw
				m_textwidget->computeMesh();
			}
		}
		
		m_cursor_position = m_textwidget->getCursorPosition(m_pos);

		// reset timer
		m_cursor_timer = 0.0f;
		m_cursor_shown = true;
		m_cursor_item->makeVisible();

		Event gui_update("gui_update", this);
		bubbleEvent("gui_redraw", &gui_update);
	}

	bool TextInput::onFocusGained(Event* ev, Unit* unit)
	{
		if (!m_text.size() && m_placeholder.size()) {
			m_label_item->setText("");
		}
		m_text_at_focus = m_text;		
		m_has_focus = true;
		updateCursor(true);
		return false;
	}

	bool TextInput::onFocusLost(Event* ev, Unit* unit)
	{
		m_has_focus = false;
		if (!m_text.size() && m_placeholder.size()) {
			m_label_item->setText(m_placeholder);
		}
		m_cursor_item->hide();

		if (m_text != m_text_at_focus) {
			// send change event
			Event commit_ev = Event("commit", this);
			sendEvent("commit", &commit_ev);
			// send to binding
			if (!m_commit_binding.target) {
				// event binding?
				if (m_values.find("onCommit") != m_values.end())
					m_commit_binding = m_app->getUnitTree()->getBinding(m_values["onCommit"].toString(), m_attributes["basepath"]);
			}
			if (m_commit_binding.target) {
				Event my_ev = Event("commit", this);
				m_commit_binding.target->sendEvent(m_commit_binding.value, &my_ev);
			}
		}

		return false;
	}

	bool TextInput::onKeyPressed(Event* ev, Unit* unit)
	{
		if (!m_has_focus)
			return false;

		InputEvent e = static_cast<EventInput*>(ev)->getEvent();
		switch (e.key) {
		case Key::Left:
			if (m_pos > 0) {
				m_pos--;
				updateCursor(true);
			}
			ev->stopPropagation();
			break;
		case Key::Right:
			if (m_pos < m_length) {
				m_pos++;
				updateCursor(true);
			}
			ev->stopPropagation();
			break;
		case Key::Tab:
			// consume only if multiline, otherwise let it propagate for focus change
			if (multiline)
				ev->stopPropagation();
			break;
		case Key::Escape:
		{
			// revert to text at focus
			m_text = m_text_at_focus;
			m_pos = m_text.size();
			m_length = m_pos;
			updateText(true);
			updateCursor(true);
			Event cancel_ev = Event("cancel", this);
			sendEvent("cancel", &cancel_ev);
			ev->stopPropagation();
			break;
		}
		case Key::Backspace:
			if (m_pos > 0) {
				m_text = m_text.substr(0, m_pos - 1) + m_text.substr(m_pos);
				m_pos--;
				m_length--;
				updateText(true);
				updateCursor();
			}
			ev->stopPropagation();
			break;
		case Key::DeleteKey:
			if (m_pos < m_length) {
				m_text = m_text.substr(0, m_pos) + m_text.substr(m_pos + 1);
				m_length--;
				updateText(true);
				updateCursor();
			}
			ev->stopPropagation();
			break;
		default:
			ev->stopPropagation();
		}

		return false;
	}

	bool TextInput::onTextInput(Event* ev, Unit* unit)
	{
		InputEvent e = static_cast<EventInput*>(ev)->getEvent();
		std::string text = e.text;
		if (text.empty()) {
			ev->stopPropagation();
			return false;
		}

		switch (static_cast<unsigned char>(text[0])) {
		case '\b':
			// ignore backspace, handled in onKeyPressed
			ev->stopPropagation();
			break;
		case '\r':
			if (multiline) {
				m_text += '\n';
				m_pos++;
				m_length++;
				updateText(true);
				updateCursor();
			}
			ev->stopPropagation();
			break;
		case 0x1B:
			// ignore escape
			ev->stopPropagation();
			break;
		default:
			std::string new_text = m_text.substr(0, m_pos);
			new_text += text;
			if (m_pos<m_length)
				new_text += m_text.substr(m_pos);
			m_text = new_text;
			m_pos += text.size();
			m_length += text.size();
			updateText(true);
			updateCursor();
			ev->stopPropagation();

		}
		return false;
	}

	bool TextInput::onMousePressed(Event* ev, Unit* unit)
	{
		// consume the mouse event
		ev->stopPropagation();

		EventInput* ei = static_cast<EventInput*>(ev);		
		int mx = ei->getEvent().x;
		int my = ei->getEvent().y;

		// get text position from label item
		if (!m_textwidget)
			return false;
		m_pos = m_textwidget->getTextPosition(Vector2((float)mx, (float)my));

		updateCursor(true);

		return false;
	}

	void TextInput::ready()
	{
		Widget::ready();

		if (m_label_item) {
			m_label_item->ready();
		}
	}

	void TextInput::updateText(bool isEvent)
	{
		if (!m_textwidget)
			return;

		m_label_item->setText(m_text);
		m_values["text"] = m_text;		

		float offset_x = m_textwidget->W() - m_textwidget->getTextWidth(m_pos);
		offset_x = (offset_x < 0.0f) ? offset_x : 0.0f;
		if (offset_x != m_offset_x) {
			m_textwidget->setOffset(offset_x);
			m_offset_x = offset_x;
			// mark for redraw
			m_textwidget->markTextDirty();
		}
		
		if (isEvent)
		{
			if (!m_change_binding.target) {
				// event binding?
				if (m_values.find("onChange") != m_values.end())
					m_change_binding = m_app->getUnitTree()->getBinding(m_values["onChange"].toString(), m_attributes["basepath"]);
			}
			// send change event

			// send to binding
			if (m_change_binding.target) {
				Event my_ev = Event("change", this);
				m_change_binding.target->sendEvent(m_change_binding.value, &my_ev);
			}

			// send to my own listeners
			Event change_ev = Event("change", this);
			sendEvent("change", &change_ev);
		}
	}
}