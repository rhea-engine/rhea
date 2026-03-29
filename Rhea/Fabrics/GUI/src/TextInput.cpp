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

namespace Rhea
{
	void TextInput::setup(Application* app, std::unordered_map<std::string, std::string> attributes, std::unordered_map<std::string, Variant> values)
	{
		Widget::setup(app, attributes, values);

		std::unordered_map<std::string, std::string> attr;
		std::unordered_map<std::string, Variant> val;

		val["text"] = getValue("text", "");
		val["templating"] = false; // dont template the user input
		m_text = val["text"];
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

		m_label_item = (Label*)m_app->getUnitTree()->createUnit(m_path + "_label", "//meworla/gui/label", attr, val);
		addChild(m_label_item);
		// store reference to textwidget
		m_textwidget = m_label_item->getTextWidget();

		// offset of "|" assumed to be 2/2
		val["x"] = -2.0f;
		val["y"] = -2.0f;
		val["w"] = 20;
		val["h"] = 20;
		m_cursor_item = (Label*)m_app->getUnitTree()->createUnit(m_path + "_cursor", "//meworla/gui/label", attr, val);
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
			(m_cursor_shown) ? m_cursor_item->makeVisble() : m_cursor_item->makeInvisble();
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
				m_textwidget->setMesh();
			}
		}
		
		m_cursor_position = m_textwidget->getCursorPosition(m_pos);

		// reset timer
		m_cursor_timer = 0.0f;
		m_cursor_shown = true;
		m_cursor_item->makeVisble();

		Event gui_update("gui_update", this);
		bubbleEvent("gui_redraw", &gui_update);
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
		if (!m_has_focus)
			return false;

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
				updateText(true);
				updateCursor();
			}
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
			else {
				SubmitEvent my_ev = SubmitEvent("submit", this);
				m_values["value"] = m_text;
				my_ev.values[m_pathname] = m_text;
				bubbleEvent("submit", &my_ev);
				if (m_values["onSubmit"] != Variant()) {
					sendBoundEvent(m_values["onSubmit"], &my_ev);
				}
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
			updateText(true);
			updateCursor();
			ev->stopPropagation();

		}
		return false;
	}

	bool TextInput::onMouseButton(Event* ev, Unit* unit)
	{
		// consume the mouse event
		ev->stopPropagation();

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
			// force redraw
			m_textwidget->setMesh();
		}
		
		if (isEvent)
		{
			if (!event_binding.target) {
				// event binding?
				if (m_values["onChange"] != Variant())
					event_binding = m_app->getUnitTree()->getBinding(m_values["onChange"], m_attributes["basepath"]);
			}
			// send change event
			if (event_binding.target) {
				Event my_ev = Event("change", this);
				event_binding.target->sendEvent(event_binding.value, &my_ev);
			}
		}
	}
}