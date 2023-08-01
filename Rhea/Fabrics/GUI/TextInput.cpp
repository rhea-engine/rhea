#include "TextInput.h"
#include "Application.h"
#include "UnitTree.h"
#include "Fabrics/Core/Scene.h"
#include "../Text/TextWidget.h"
#include "Label.h"
#include <Psio/Log.h>
#include <Psio/Keyboard.h>
#include "EventInput.h"

namespace Rhea
{
	void TextInput::setup(Application* app, std::unordered_map<std::string, std::string> attributes, std::unordered_map<std::string, Variant> values)
	{
		Widget::setup(app, attributes, values);

		std::unordered_map<std::string, std::string> attr;
		std::unordered_map<std::string, Variant> val;

		val["x"] = 0.0f;
		val["y"] = 0.0f;
		val["w"] = m_base_size.x;
		val["h"] = m_base_size.y;
		val["text"] = getValue("text", "");
		m_text = val["text"];
		val["placeholder"] = getValue("placeholder", "");
		// multiline is not implemented yet
		val["multiline"] = false;
		val["word_wrap"] = false;
		m_placeholder = val["placeholder"];
		m_pos = m_text.size();
		if (!m_pos) {
			val["text"] = m_placeholder;
		}
		m_length = m_pos;
		if (m_values.find("font") != m_values.end())
			val["font"] = getValue("font");

		// DEBUG
		// always handle input 
		m_handle_input = true;

		m_label_item = (Label*)m_app->getUnitTree()->createUnit(m_path + "/label", "//meworla/gui/label", attr, val);
		addChild(m_label_item);
		// store reference to textwidget
		m_textwidget = m_label_item->getTextWidget();

		// offset of "|" assumed to be 2/2
		val["x"] = -2.0f;
		val["y"] = -2.0f;
		val["w"] = 20;
		val["h"] = 20;
		m_cursor_item = (Label*)m_app->getUnitTree()->createUnit(m_path + "/cursor", "//meworla/gui/label", attr, val);
		addChild(m_cursor_item);
		m_cursor_item->setText("|");
		m_cursor_item->hide();

		// register to focus events
		registerEventHandler("focus", this, std::bind(&TextInput::onFocus, this, std::placeholders::_1, std::placeholders::_2));
		registerEventHandler("focus_lost", this, std::bind(&TextInput::onLooseFocus, this, std::placeholders::_1, std::placeholders::_2));
		registerEventHandler("key_pressed", this, std::bind(&TextInput::onKeyPressed, this, std::placeholders::_1, std::placeholders::_2));
		registerEventHandler("text_input", this, std::bind(&TextInput::onTextInput, this, std::placeholders::_1, std::placeholders::_2));
		registerEventHandler("mouse_btn_down", this, std::bind(&TextInput::onMouseButton, this, std::placeholders::_1, std::placeholders::_2));		
	}

	TextInput* TextInput::createType(std::string name, std::string type_name)
	{
		TextInput* ret = new TextInput(name, type_name);
		return ret;
	}

	void TextInput::setText(std::string text)
	{
		m_text = text;
		m_pos = m_text.size();
		m_length = m_pos;
		updateText();
	}

	void TextInput::onFrame(float delta)
	{
		if (!m_has_focus)
			return;

		m_cursor_timer += delta;
		if (m_cursor_timer >= 0.7f) {
			m_cursor_shown = !m_cursor_shown;
			m_cursor_timer = 0.0f;
			(m_cursor_shown) ? m_cursor_item->show() : m_cursor_item->hide();
		}
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
				m_textwidget->setMesh();
			}
		}
		
		Vector2 px_pos = m_textwidget->getCursorPosition(m_pos);
		m_cursor_item->move(px_pos.x-2.0f, px_pos.y);
		m_cursor_item->setArea(true, false);

		// reset timer
		m_cursor_timer = 0.0f;
		m_cursor_shown = true;
		m_cursor_item->show();
	}

	bool TextInput::onFocus(Event* ev, Unit* unit)
	{
		if (!m_text.size() && m_placeholder.size()) {
			m_label_item->setText("");
		}
		m_has_focus = true;
		updateCursor(true);
		return false;
	}

	bool TextInput::onLooseFocus(Event* ev, Unit* unit)
	{
		m_has_focus = false;
		if (!m_text.size() && m_placeholder.size()) {
			m_label_item->setText(m_placeholder);
		}
		m_cursor_item->hide();
		return false;
	}

	bool TextInput::onKeyPressed(Event* ev, Unit* unit)
	{
		Psio::KeyInputEvent* e = static_cast<Psio::KeyInputEvent*>(static_cast<EventInput*>(ev)->getEvent().get());
		switch (e->getKeyCode()) {
		case Psio::Key::CODE_LEFT:
			if (m_pos > 0) {
				m_pos--;
				updateCursor(true);
				
			}
			ev->stopPropagation();
			break;
		case Psio::Key::CODE_RIGHT:
			if (m_pos < m_length) {
				m_pos++;
				updateCursor(true);
			}
			ev->stopPropagation();
			break;
		}
		return false;
	}

	bool TextInput::onTextInput(Event* ev, Unit* unit)
	{
		std::shared_ptr<Psio::InputEvent> e = static_cast<EventInput*>(ev)->getEvent();
		uint8_t code = static_cast<Psio::KeyInputEvent*>(e.get())->getCode();
		switch (code) {
		case '\b':
			if (m_pos > 0) {
				m_text = m_text.substr(0, m_pos - 1) + m_text.substr(m_pos);
				m_pos--;
				m_length--;
				updateText();
				updateCursor();
			}
			ev->stopPropagation();
			break;
		case '\r':
			if (multiline) {
				m_text += '\n';
				m_pos++;
				m_length++;
				updateText();
				updateCursor();
			}
			else {
				Event my_ev = Event("submitted", this);
				bubbleEvent("submitted", &my_ev);
			}
			ev->stopPropagation();
			break;
		default:
			std::string new_text = m_text.substr(0, m_pos);
			new_text += code;
			if (m_pos<m_length)
				new_text += m_text.substr(m_pos);
			m_text = new_text;
			m_pos++;
			m_length++;
			updateText();
			updateCursor();
			ev->stopPropagation();

		}
		return false;
	}

	bool TextInput::onMouseButton(Event* ev, Unit* unit)
	{
		EventInput* ei = static_cast<EventInput*>(ev);
		std::shared_ptr<Psio::MouseInputEvent> e = std::static_pointer_cast<Psio::MouseInputEvent>(ei->getEvent());
		int mx = e->getX();
		int my = e->getY();

		// get text position from label item
		if (!m_textwidget)
			return false;
		m_pos = m_textwidget->getTextPosition(Vector2((float)mx, (float)my));

		updateCursor(true);

		return false;
	}

	void TextInput::setArea(bool moved, bool sized)
	{
		Widget::setArea(moved, sized);

		if (m_label_item) {
			if (moved) {
				DLOG(LOG_DEBUG) << "GUI::TextInput: widget " << m_pathname << ": screen(x,y)=" << m_position.x << ", " << m_position.y << ")";
				m_label_item->move(m_position.x, m_position.y);
			}
			if (sized) {
				DLOG(LOG_DEBUG) << "GUI::TextInput: widget " << m_pathname << ": size(w,h)" << m_size.x << ", " << m_size.y << ")";
				m_label_item->resize(m_size.x, m_size.y);
			}
			if (sized || moved) {
				m_label_item->update();
			}
		}
	}

	void TextInput::ready()
	{
		Widget::ready();

		if (m_label_item) {
			m_label_item->ready();
		}
	}

	void TextInput::updateText()
	{
		if (!m_textwidget)
			return;

		m_label_item->setText(m_text);

		float offset_x = m_textwidget->W() - m_textwidget->getTextWidth(m_pos);
		offset_x = (offset_x < 0.0f) ? offset_x : 0.0f;
		if (offset_x != m_offset_x) {
			m_textwidget->setOffset(offset_x);
			m_offset_x = offset_x;
			// force redraw
			m_textwidget->setMesh();
		}
		
	}
}