/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#include <Rhea/GUI/Dialog.h>
#include <Rhea/Application.h>
#include <Rhea/UnitTree.h>
#include <Psio/Log.h>
#include <Rhea/EventInput.h>

#include <Rhea/GUI/Modifier/FillSize.h>
#include <Rhea/GUI/TextInput.h>
#include <Rhea/GUI/Button.h>

namespace rhea
{
	void Dialog::setup(Application* app, std::unordered_map<std::string, std::string> attributes, std::unordered_map<std::string, Variant> values)
	{
		Widget::setup(app, attributes, values);

		std::unordered_map<std::string, std::string> attr;
		std::unordered_map<std::string, Variant> val;
	}

	void Dialog::ready()
	{
		Widget::ready();

		registerEventHandler("close", this, [this](Event* ev, Unit* u)->bool {
			hide();
			return false;
			});

		// semantic events

		registerEventHandler("submit", this, [this](Event* ev, Unit* u)->bool {
			submit();
			ev->stopPropagation();
			return true;
			});
		registerEventHandler("cancel", this, [this](Event* ev, Unit* u)->bool {
			cancel();
			ev->stopPropagation();
			return true;
			});

		m_default_button = findDefaultButton();

		// dialogs are not visible by default and must be "opened"
		makeInvisible();
	}

	bool Dialog::onKeyPressed(Event* ev, Unit* u)
	{
		EventInput* input_ev = static_cast<EventInput*>(ev);
		// esc cancels the dialog
		if (input_ev && input_ev->isAction("cancel")) {
			cancel();
			ev->stopPropagation();
			return true;
		}
		// return submits the dialog
		if (input_ev && input_ev->isAction("submit"))
		{
			submit();
			ev->stopPropagation();
			return true;
		}
		return false;
	}

	void Dialog::cancel()
	{
		Event ev("canceled", this);
		if (m_values.find("onCancel") != m_values.end()) {
			sendBoundEvent(m_values["onCancel"].toString(), &ev);
		}
		else
			sendEvent("canceled", &ev);
	}

	void Dialog::submit()
	{
		Event ev("submitted", this);
		if (m_values.find("onSubmit") != m_values.end()) {
			sendBoundEvent(m_values["onSubmit"].toString(), &ev);
		}
		else
			sendEvent("submitted", &ev);
	}

	void Dialog::show()
	{
		Widget::show();
		m_finished = false;
	}

	// a dialog centers itself in the parents width 
	void Dialog::onSetPosition()
	{
		Widget* p = doCast<Widget>(m_parent);
		if (p) {
			Vector2 parent_size = p->getSize();

			// m_outer_size contains the size including margin
			// Calculate padding to center the dialog
			Vector2 padding = Vector2((parent_size.x - m_outer_size.x) / 2, (parent_size.y - m_outer_size.y) / 2);

			// Override the position calculation:
			// Instead of using anchor + base_position, we use anchor + centering offset
			// This means we ignore any authored x/y offset (m_base_position)
			m_position = m_anchor_position + padding;
		}

		// set children position
		Widget::onSetPosition();
	}

	Widget* Dialog::findInitialFocusWidget()
	{
		if (m_initial_focus_widget && m_initial_focus_widget->isFocusable())
			return m_initial_focus_widget;

		Widget* first_focusable = nullptr;
		Widget* first_text_input = nullptr;

		traverse([this,&first_focusable,&first_text_input](std::string& name, Component* c)->bool {
			if (c == this) return true; // skip self
			Widget* w = doCast<Widget>(c);
			if (w && w->isFocusable()) {
				
				if (!first_focusable)
					first_focusable = w;

				if (!first_text_input && doCast<TextInput>(w))
					first_text_input = w;

				if (first_focusable && first_text_input) 
					return false; // stop traversal
				
				return true; // continue traversal
				}
			return true; // continue traversal
			});

		if (first_text_input)
			return first_text_input;

		if (m_default_button && m_default_button->isFocusable() &&
			m_default_button->isEnabled() && m_default_button->isVisible())
			return m_default_button;

		if (first_focusable)
			return first_focusable;

		return this;
	}

	Widget* Dialog::findDefaultButton() const
	{
		Widget* first_button = nullptr;
		Widget* default_button = nullptr;

		const_cast<Dialog*>(this)->traverse([this,&first_button,&default_button](std::string& name, Component* c)->bool {
			if (c == this) return true; // skip self
			Button* btn = doCast<Button>(c);
			if (!btn || !btn->isEnabled() || !btn->isVisible())
				return true;

			if (btn->isDefault()) {
				default_button = btn;
				return false; // stop traversal
			}
			
			if (!first_button)
				first_button = btn;

			return true; // continue traversal
			});

		if (default_button)
			return default_button;

		return first_button;
	}
}