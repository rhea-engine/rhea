/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#include <Psio/Input/InputEvent.h>
#include <functional>

namespace psio
{
	std::shared_ptr<InputEvent> InputEvent::createEvent(InputEventId id)
	{
		std::shared_ptr<InputEvent> evt;

		switch (id) {
		case InputEventId::NONE:
			return NULL;
		case InputEventId::KEY_PRESSED:
		case InputEventId::KEY_RELEASED:		
			evt = std::make_shared<KeyInputEvent>(id);
			return evt;
			break;
		case InputEventId::TEXT:
			evt = std::make_shared<TextInputEvent>("");
			return evt;
			break;
		case InputEventId::MOUSE_MOVE:
		case InputEventId::MOUSE_BTN_PRESSED:
		case InputEventId::MOUSE_BTN_RELEASED:
		case InputEventId::MOUSE_BTN_DBLCLICK:
			return std::make_shared<MouseInputEvent>(id);
			break;
		case InputEventId::MOUSE_WHEEL:
			return std::make_shared<MouseWheelEvent>();
		case InputEventId::WMGR_QUIT:
		case InputEventId::WIN_SIZE:
		default:
			evt = std::make_shared<InputEvent>(id);
			return evt;
		}
	}

	void InputEvent::setModifiers(bool shift, bool alt, bool ctrl)
	{
		m_shift = shift;
		m_alt = alt;
		m_ctrl = ctrl;
		m_hash = 0;
	}

	void InputEvent::updateHash(int raw)
	{
		raw |= static_cast<int>(m_id) & 0xFF;
		raw |= (m_shift) ? 1 << 8 : 0;
		raw |= (m_alt) ? 1 << 9 : 0;
		raw |= (m_ctrl) ? 1 << 10 : 0;
		m_hash = std::hash<int>()(raw);
	}
	
	bool KeyInputEvent::isEvent(InputEvent* evt)
	{
		// make sure we have the matching id
		if (!evt || evt->getId() != m_id)
			return false;

		KeyInputEvent* e = dynamic_cast<KeyInputEvent*>(evt);
		if (!e)
			return false;

		if (e->m_id == InputEventId::KEY_RELEASED)
		{
			// no modifiers check
			if (m_keycode)
				return e->m_keycode == m_keycode;
			else
				return e->m_physical_keycode == m_physical_keycode;
		}
		else {
			if (m_keycode)
				return e->m_keycode == m_keycode && e->m_alt == m_alt && e->m_shift == m_shift && e->m_ctrl == m_ctrl;
			else
				return e->m_physical_keycode == m_physical_keycode && e->m_alt == m_alt && e->m_shift == m_shift && e->m_ctrl == m_ctrl;
		}
	}

	void KeyInputEvent::updateHash(int raw)
	{
		if (m_keycode)
			raw |= static_cast<int>(m_keycode << 16);
		else
			raw |= static_cast<int>(m_physical_keycode << 16);
		InputEvent::updateHash(raw);
	}
	
	void KeyInputEvent::set(uint32_t keycode, uint32_t physical_keycode, uint8_t unicode)
	{
		m_keycode = keycode;
		m_physical_keycode = physical_keycode;
		m_unicode = unicode;
		m_hash = 0;
	}
	
	bool TextInputEvent::isEvent(InputEvent* evt)
	{
		// make sure we have the matching id
		if (!evt || evt->getId() != m_id)
			return false;
		auto* e = dynamic_cast<TextInputEvent*>(evt);
		if (!e)
			return false;
		return e->m_text == m_text && e->m_alt == m_alt && e->m_shift == m_shift && e->m_ctrl == m_ctrl;
	}

	bool MouseInputEvent::isEvent(InputEvent* evt)
	{
		// make sure we have the matching id
		if (!evt || evt->getId() != m_id)
			return false;

		auto* e = dynamic_cast<MouseInputEvent*>(evt);
		if (!e)
			return false;

		return e->m_left == m_left && e->m_right == m_right && e->m_middle == m_middle
			&& e->m_alt == m_alt && e->m_shift == m_shift && e->m_ctrl == m_ctrl;
	}

	void MouseInputEvent::updateHash(int raw)
	{
		raw |= (m_left) ? 1 << 16 : 0;
		raw |= (m_right) ? 1 << 17 : 0;
		raw |= (m_middle) ? 1 << 18 : 0;
		InputEvent::updateHash(raw);
	}

	void MouseInputEvent::setBtn(bool left, bool right, bool middle)
	{
		m_left = left;
		m_right = right;
		m_middle = middle;
		m_hash = 0;
	}

	void MouseInputEvent::setPos(int x, int y)
	{
		m_x = x;
		m_y = y;
		m_hash = 0;
	}

	void MouseWheelEvent::setPos(int x, int y)
	{
		m_x = x;
		m_y = y;
		m_hash = 0;
	}

	void MouseWheelEvent::setDelta(int wheelY, int wheelX)
	{
		m_wheelY = wheelY;
		m_wheelX = wheelX;
		m_hash = 0;
	}

	bool MouseWheelEvent::isEvent(InputEvent* evt)
	{
		// make sure we have the matching id
		if (!evt || evt->getId() != m_id)
			return false;
		auto* e = dynamic_cast<MouseWheelEvent*>(evt);
		if (!e)
			return false;
		return e->m_wheelY == m_wheelY && e->m_wheelX == m_wheelX
			&& e->m_alt == m_alt && e->m_shift == m_shift && e->m_ctrl == m_ctrl;
	}

	void MouseWheelEvent::updateHash(int raw)
	{
		raw |= static_cast<int>(m_wheelY << 16);
		raw |= static_cast<int>(m_wheelX << 24);
		InputEvent::updateHash(raw);
	}
}