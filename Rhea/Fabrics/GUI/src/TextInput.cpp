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
#include <Rhea/Platform/PlatformService.h>
#include <Psio/Window/Clipboard.h>
#include <algorithm>
#include <cctype>
#include <cmath>

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
		m_selection_anchor = m_pos;
		if (m_values.find("font") != m_values.end())
			val["font"] = getValue("font");

		// A non-widget decoration does not intercept mouse input or affect layout.
		m_selection_background = static_cast<Item2D*>(m_app->getUnitTree()->createUnit(getPathName() + "_selection", "//meworla/Item2D", attr, {}));
		m_selection_background->setBackgroundColor(65, 110, 170);
		addChild(m_selection_background);

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
		m_selection_anchor = m_pos;
		updateText(false);
		updateCursor();
	}

	void TextInput::onFrame(float delta)
	{
		m_double_click_timer += delta;
		if (!m_has_focus)
			return;
		updateSelection();

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
		updateScrollOffset();
		m_cursor_position = m_textwidget->getCursorPosition(m_pos);
		m_cursor_item->setPosition(m_cursor_position);
		updateSelection();
	}

	void TextInput::updateCursor(bool onlyCursor)
	{
		if (!m_textwidget)
			return;

		updateScrollOffset();
		
		m_cursor_position = m_textwidget->getCursorPosition(m_pos);

		// reset timer
		m_cursor_timer = 0.0f;
		m_cursor_shown = true;
		if (m_has_focus)
			m_cursor_item->makeVisible();
		m_cursor_item->setPosition(m_cursor_position);
		updateSelection();

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
		m_selecting = false;
		m_double_click_timer = 1.0f;
		updateSelection();
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

	std::string TextInput::getSelectedText() const
	{
		const size_t first = (std::min)(m_selection_anchor, m_pos);
		return m_text.substr(first, (std::max)(m_selection_anchor, m_pos) - first);
	}

	void TextInput::selectAll()
	{
		m_selection_anchor = 0;
		m_pos = m_text.size();
		updateCursor(true);
	}

	void TextInput::moveCursor(size_t position, bool extend_selection)
	{
		m_pos = (std::min)(position, m_text.size());
		if (!extend_selection)
			m_selection_anchor = m_pos;
		updateCursor(true);
	}

	void TextInput::replaceSelection(const std::string& text)
	{
		const size_t first = (std::min)(m_selection_anchor, m_pos);
		const size_t last = (std::max)(m_selection_anchor, m_pos);
		if (first == last && text.empty())
			return;
		m_text.replace(first, last - first, text);
		m_pos = first + text.size();
		m_selection_anchor = m_pos;
		m_length = m_text.size();
		updateText(true);
		updateCursor(true);
	}

	bool TextInput::onKeyPressed(Event* ev, Unit* unit)
	{
		if (!m_has_focus)
			return false;

		const InputEvent e = static_cast<EventInput*>(ev)->getEvent();
		m_double_click_timer = 1.0f;
		if (e.modifiers.ctrl && !e.modifiers.alt) {
			if (e.key == Key::A) {
				selectAll();
				ev->stopPropagation();
				return false;
			}
			if (e.key == Key::C || e.key == Key::X || e.key == Key::V) {
				auto* platform = m_app ? m_app->getService<PlatformService>() : nullptr;
				auto* clipboard = platform && platform->getEngine() ? platform->getEngine()->getClipboard() : nullptr;
				if (clipboard) {
					if (e.key == Key::V) {
						std::string clip_text;
						if (clipboard->getText(clip_text) && !clip_text.empty()) {
							// TextInput is currently single-line.
							clip_text.erase(std::remove_if(clip_text.begin(), clip_text.end(), [](unsigned char c) { return c < 0x20 || c == 0x7f; }), clip_text.end());
							if (!clip_text.empty())
								replaceSelection(clip_text);
						}
					}
					else if (hasSelection() && clipboard->setText(getSelectedText()) && e.key == Key::X)
						replaceSelection("");
				}
				ev->stopPropagation();
				return false;
			}
		}
		switch (e.key) {
		case Key::Left:
		{
			const size_t position = !e.modifiers.shift && hasSelection()
				? (std::min)(m_selection_anchor, m_pos)
				: (m_pos > 0 ? m_pos - 1 : 0);
			moveCursor(position, e.modifiers.shift);
			break;
		}
		case Key::Right:
		{
			const size_t position = !e.modifiers.shift && hasSelection()
				? (std::max)(m_selection_anchor, m_pos)
				: (std::min)(m_pos + 1, m_length);
			moveCursor(position, e.modifiers.shift);
			break;
		}
		case Key::Home:
			moveCursor(0, e.modifiers.shift);
			break;
		case Key::End:
			moveCursor(m_length, e.modifiers.shift);
			break;
		case Key::Tab:
			if (!multiline)
				return false;
			break;
		case Key::Escape:
		{
			m_text = m_text_at_focus;
			m_pos = m_text.size();
			m_length = m_pos;
			m_selection_anchor = m_pos;
			updateText(true);
			updateCursor(true);
			Event cancel_ev("cancel", this);
			sendEvent("cancel", &cancel_ev);
			break;
		}
		case Key::Enter:
		{
			if (!multiline) {
				Event on_submit_ev("submit", this);
				if (m_values.find("onSubmit") != m_values.end())
					sendBoundEvent(m_values["onSubmit"].toString(), &on_submit_ev);
				Event submit_ev("submit", this);
				bubbleEvent("submit", &submit_ev);
			}
			break;
		}
		case Key::Backspace:
			if (!hasSelection() && m_pos > 0)
				m_selection_anchor = m_pos - 1;
			replaceSelection("");
			break;
		case Key::DeleteKey:
			if (!hasSelection() && m_pos < m_length)
				m_selection_anchor = m_pos + 1;
			replaceSelection("");
			break;
		default:
			break;
		}
		ev->stopPropagation();
		return false;
	}

	bool TextInput::onTextInput(Event* ev, Unit* unit)
	{
		if (!m_has_focus)
			return false;
		const InputEvent e = static_cast<EventInput*>(ev)->getEvent();
		ev->stopPropagation();
		if (e.modifiers.ctrl || e.modifiers.alt)
			return false;
		std::string text = e.text;
		text.erase(std::remove_if(text.begin(), text.end(), [](unsigned char c) { return c < 0x20 || c == 0x7f; }), text.end());
		if (!text.empty())
			replaceSelection(text);
		return false;
	}

	bool TextInput::onMousePressed(Event* ev, Unit* unit)
	{
		const InputEvent e = static_cast<EventInput*>(ev)->getEvent();
		if (e.mouseButton != MouseButton::Left || !m_textwidget)
			return false;
		m_selecting = true;
		moveCursor(m_textwidget->getTextPosition(Vector2((float)e.x, (float)e.y)), e.modifiers.shift);
		ev->stopPropagation();
		return false;
	}

	bool TextInput::onMouseReleased(Event* ev, Unit* unit)
	{
		if (static_cast<EventInput*>(ev)->getEvent().mouseButton != MouseButton::Left)
			return false;
		if (m_selecting)
			onDragMove(ev, unit);
		m_selecting = false;
		ev->stopPropagation();
		return false;
	}

	bool TextInput::onDragStart(Event* ev, Unit* unit)
	{
		return onDragMove(ev, unit);
	}

	bool TextInput::onDragMove(Event* ev, Unit* unit)
	{
		if (!m_selecting || !m_has_focus || !m_textwidget)
			return false;
		const InputEvent e = static_cast<EventInput*>(ev)->getEvent();
		moveCursor(m_textwidget->getTextPosition(Vector2((float)e.x, (float)e.y)), true);
		if (e.type == InputEventType::MouseMoved)
			m_double_click_timer = 1.0f;
		ev->stopPropagation();
		return false;
	}

	bool TextInput::onDragEnd(Event* ev, Unit* unit)
	{
		if (!m_selecting)
			return false;
		onDragMove(ev, unit);
		m_selecting = false;
		return false;
	}

	void TextInput::selectWord(size_t position)
	{
		if (m_text.empty())
			return;
		position = (std::min)(position, m_text.size() - 1);
		// Keep file names (including dots, hyphens and underscores) together.
		auto category = [](unsigned char c) {
			if (std::isspace(c))
				return 0;
			if (c == '/' || c == '\\' || c == ':' || c == '?' || c == '#' || c == '&' || c == '=')
				return 1;
			return 2;
		};
		const int selected_category = category(m_text[position]);
		size_t first = position;
		size_t last = position + 1;
		while (first > 0 && category(m_text[first - 1]) == selected_category)
			--first;
		while (last < m_text.size() && category(m_text[last]) == selected_category)
			++last;
		m_selection_anchor = first;
		m_pos = last;
		updateCursor(true);
	}

	bool TextInput::onMouseDoubleClicked(Event* ev, Unit* unit)
	{
		const InputEvent e = static_cast<EventInput*>(ev)->getEvent();
		if (e.mouseButton != MouseButton::Left || !m_textwidget)
			return false;
		const bool repeated = m_double_click_timer < 0.7f
			&& std::abs(e.x - m_double_click_position.x) <= 4.0f
			&& std::abs(e.y - m_double_click_position.y) <= 4.0f;
		if (repeated)
			selectAll();
		else
			selectWord(m_textwidget->getTextPosition(Vector2((float)e.x, (float)e.y)));
		m_double_click_timer = repeated ? 1.0f : 0.0f;
		m_double_click_position = Vector2((float)e.x, (float)e.y);
		m_selecting = false;
		ev->stopPropagation();
		return false;
	}

	void TextInput::updateScrollOffset()
	{
		if (!m_textwidget || !m_textwidget->getFont())
			return;
		const float width = (std::max)(0.0f, m_textwidget->W());
		const float cursor_x = m_textwidget->getTextWidth(m_pos);
		float offset_x = m_offset_x;
		if (cursor_x + offset_x < 0.0f)
			offset_x = -cursor_x;
		else if (cursor_x + offset_x > width)
			offset_x = width - cursor_x;
		const float max_scroll = (std::max)(0.0f, m_textwidget->getTextWidth(m_length) - width);
		offset_x = (std::max)(-max_scroll, (std::min)(offset_x, 0.0f));
		if (offset_x != m_offset_x) {
			m_offset_x = offset_x;
			m_textwidget->setOffset(offset_x);
			m_textwidget->markTextDirty();
		}
	}

	void TextInput::updateSelection()
	{
		if (!m_selection_background || !m_textwidget)
			return;
		auto* visual = m_selection_background->getComponent<VisualComponent>();
		if (!visual)
			return;
		if (!m_has_focus || !hasSelection() || !m_textwidget->getFont()) {
			if (visual->isVisible())
				m_selection_background->hide();
			return;
		}
		const Vector2 first = m_textwidget->getCursorPosition((std::min)(m_selection_anchor, m_pos));
		const Vector2 last = m_textwidget->getCursorPosition((std::max)(m_selection_anchor, m_pos));
		const float left = (std::max)(first.x, m_textwidget->X());
		const float right = (std::min)(last.x, m_textwidget->X() + m_textwidget->W());
		const float top = m_textwidget->Y();
		const float height = (std::min)(m_textwidget->getFont()->getLineHeight(), m_textwidget->H());
		if (right <= left || height <= 0.0f) {
			if (visual->isVisible())
				m_selection_background->hide();
			return;
		}
		if (!visual->isVisible())
			m_selection_background->show();
		if (m_selection_background->X() != left || m_selection_background->Y() != top
			|| m_selection_background->W() != right - left || m_selection_background->H() != height) {
			m_selection_background->move(left, top);
			m_selection_background->resize(right - left, height);
			m_selection_background->invalidateRender();
		}
		auto* text_visual = m_textwidget->getComponent<VisualComponent>();
		if (visual && text_visual) {
			if (visual->hasClipping() != text_visual->hasClipping())
				visual->enableClipping(text_visual->hasClipping());
			if (text_visual->hasClipping()) {
				const ClipRect old_clip = visual->getClipping();
				const ClipRect clip = text_visual->getClipping();
				if (old_clip.x != clip.x || old_clip.y != clip.y || old_clip.w != clip.w || old_clip.h != clip.h)
					visual->setClipping(clip);
			}
		}
	}

	void TextInput::ready()
	{
		Widget::ready();
		if (m_selection_background) {
			m_selection_background->getComponent<VisualComponent>()->setRenderPhase(RenderPhase::Overlay);
			m_selection_background->hide();
		}

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

		updateScrollOffset();
		
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
