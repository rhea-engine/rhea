/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#include <Rhea/GUI/List.h>
#include <Rhea/GUI/ListItem.h>
#include <Rhea/Application.h>
#include <Rhea/UnitTree.h>
#include <Psio/Input/Keyboard.h>
#include <Rhea/EventInput.h>
#include <algorithm>

namespace rhea
{
	void rhea::ListWidget::setup(Application* app, std::unordered_map<std::string, std::string> attributes, std::unordered_map<std::string, Variant> values)
	{
		Widget::setup(app, attributes, values);		
	}

	void ListWidget::addItem(std::string text, std::string value)
	{
		std::unordered_map<std::string, std::string> attributes;
		std::unordered_map<std::string, Variant> values;

		values["padding"] = getValue("item_padding", 0.0f);
		values["text"] = text;
		values["value"] = value;
		values["fillwidth"] = true;

		ListWidgetItem* item = (ListWidgetItem*)m_app->getUnitTree()->createUnit("child_" + std::to_string(1 + m_list_items.size()), "//meworla/gui/listitem", attributes, values);
		addChild(item);
		m_list_items.push_back(item);
		item->ready();

		invalidateLayout();
	}

	void ListWidget::clear()
	{
		// iterate over children and delete all listwidgetitems
		std::vector<ListWidgetItem*> to_del;
		for (auto it = m_children.begin(); it != m_children.end(); ++it) {
			ListWidgetItem* w = doCast<ListWidgetItem>(it->second);
			if (w) {
				to_del.push_back(w);
			}
		}

		// clear cache
		m_list_items.clear();
		m_selected_item = nullptr;
		m_selected_value.clear();

		for (ListWidgetItem* w : to_del) {
			removeChild(w);
			Event e("scene_leave", w);
			bubbleEvent("scene_leave", &e);
			w->destroy();
		}
	}

	void ListWidget::ready()
	{
		Widget::ready();

		// check if we have a data binding
		if (m_values["data"] != Variant()) {
			m_data_binding = m_app->getUnitTree()->getBinding(m_values["data"].toString(), m_attributes["basepath"]);

			// fill the list once
			if (m_data_binding.target) {
				pollData();
			}
		}

		registerEventHandler("selected", this, std::bind(&ListWidget::onSelected, this, std::placeholders::_1, std::placeholders::_2));
		registerEventHandler("activated", this, std::bind(&ListWidget::onActivated, this, std::placeholders::_1, std::placeholders::_2));
		registerEventHandler("clicked", this, std::bind(&ListWidget::onMouseClicked, this, std::placeholders::_1, std::placeholders::_2));
	}

	void ListWidget::onFrame(float delta)
	{
		// rebuild item list
		if (m_data_binding.target) {
			pollData();
		}
	}

	// this polls the bound data
	// change is detected by comparing the value
	// if at least one value is not the same rebuild the list
	// all children are deleted and the list is created from scratch
	void ListWidget::pollData()
	{
		Variant items = m_data_binding.target->getValue(m_data_binding.value);
		if (items != Variant()) {
			// changed?
			bool changed = false;
			// store current selection by value			
			if (m_selected_item)
				m_selected_value = (std::string)m_selected_item->getValue("value");

			if (m_list_items.size()) {
				if (m_list_items.size() == items.length()) {
					for (int i = 0; i < items.length(); i++) {
						if (std::string(m_list_items[i]->getValueString("value")).compare(items[i].toString()))
							changed = true;
					}
				}
				else
					changed = true;
				if (changed) {
					clear();									
				}
			}
			else
				changed = true;
			if (changed) {
				m_selected_item = nullptr;
				for (int i = 0; i < items.length(); i++) {
					Variant item = items[i];
					addItem(item.toString(), item.toString());
				}
				if (!m_selected_value.empty()) {
					// restore selection by value
					for (ListWidgetItem* item : m_list_items) {
						if (item->getValueString("value") == m_selected_value) {
							selectItem(item);
							break;
						}
					}
				}
			}
		}
	}

	void ListWidget::selectItem(ListWidgetItem* item)
	{
		if (item == m_selected_item)
			return;

		if (m_selected_item) {
			m_selected_item->setWidgetState(Widget::WidgetState::kFocus, false);
		}
		m_selected_item = item;
		m_selected_value = item ? item->getValueString("value") : "";

		if (item)
			item->setWidgetState(WidgetState::kFocus, true);
		invalidateLayout();


		Event change_ev = Event("selection_changed", m_selected_item);
		sendEvent("selection_changed", &change_ev);

		if (!change_ev.isPropagationStopped() && m_values.find("onSelected") != m_values.end()) {
			sendBoundEvent(m_values["onSelected"].toString(), &change_ev);
		}
	}

	bool ListWidget::onSelected(Event* ev, Unit* unit)
	{
		ev->stopPropagation();		

		// selected item in list is the event target
		ListWidgetItem* item = doCast<ListWidgetItem>(ev->getTarget());
		selectItem(item);
		return false;
	}

	bool ListWidget::onActivated(Event* ev, Unit* unit)
	{
		ev->stopPropagation();

		Event my_ev = Event("item_activated", ev->getTarget());
		sendEvent("item_activated", &my_ev);

		if (!my_ev.isPropagationStopped() && m_values.find("onActivated") != m_values.end()) {
			sendBoundEvent(m_values["onActivated"].toString(), &my_ev);
		}

		if (!my_ev.isPropagationStopped() && m_values.find("action") != m_values.end()) {
			sendEvent(m_values["action"].toString(), &my_ev);
		}
		return false;
	}

	bool ListWidget::onMouseClicked(Event* ev, Unit* unit)
	{
		ev->stopPropagation();

		ListWidgetItem* item = doCast<ListWidgetItem>(ev->getTarget());

		Event click_ev = Event("item_clicked", item);
		sendEvent("item_clicked", &click_ev);

		if (!click_ev.isPropagationStopped() && m_values.find("onClicked") != m_values.end()) {
			sendBoundEvent(m_values["onClicked"].toString(), &click_ev);
		}

		return false;
	}

	void ListWidget::onSetPosition()
	{
		m_cur_position = m_position + m_content_position;

		Vector2 child_position = m_position + m_content_position;
		for (auto it = m_list_items.begin(); it != m_list_items.end(); ++it) {
			Widget* w = doCast<Widget>(*it);
			if (w) {
				w->setPosition(child_position);
				child_position.y += w->H();
			}
		}
	}

	// the list widget inner size is the max width of its children
	// and the sum of its childrens heights
	Vector2 ListWidget::innerSize(Vector2 size_constraints)
	{
		Vector2 innerSize(0.0f, 0.0f);

		for (auto it = m_children.begin(); it != m_children.end(); ++it) {
			ListWidgetItem* li = doCast<ListWidgetItem>(it->second);
			if (li) {
				Vector2 child_size = li->size(size_constraints);
				if (child_size.x > innerSize.x)
					innerSize.x = child_size.x;
				innerSize.y += child_size.y;
			}
		}
		return innerSize;
	}

	bool ListWidget::onFocusGained(Event* ev, Unit* unit)
	{
		Widget::onFocusGained(ev, unit);
		if (m_selected_item) {
			m_selected_item->setWidgetState(Widget::WidgetState::kFocus, true);
			invalidateLayout();
		}
		return true;
	}

	bool ListWidget::onFocusLost(Event* ev, Unit* unit)
	{
		Widget::onFocusLost(ev, unit);
		if (m_selected_item) {
			m_selected_item->setWidgetState(Widget::WidgetState::kFocus, false);
			invalidateLayout();
		}
		return true;
	}

	bool ListWidget::onKeyPressed(Event* ev, Unit* unit)
	{
		if (!hasFocus())
			return false;

		EventInput* inputEv = dynamic_cast<EventInput*>(ev);
		if (!inputEv)
			return false;
		InputEvent e = inputEv->getEvent();

		switch (e.key) {
		case Key::Up:
		
			ev->stopPropagation();			
			moveActive(-1);			
			return true;
		
		case Key::Down:
		
			ev->stopPropagation();		
			moveActive(1);			
			return true;
		
		case Key::Enter:
		case Key::Space:
		{
			ev->stopPropagation();
			if (m_selected_item) {
				// send activated event
				Event my_ev = Event("activated", m_selected_item);
				onActivated(&my_ev, m_selected_item);
			}
			return true;
		}
		default:
			return false;
		}
	}

	void ListWidget::moveActive(int direction)
	{
		if (m_list_items.empty())
			return;
		
		int current_index = -1;
		if (m_selected_item) {
			auto it = std::find(m_list_items.begin(), m_list_items.end(), m_selected_item);
			if (it != m_list_items.end()) {
				current_index = static_cast<int>(std::distance(m_list_items.begin(), it));
			}
		}
		
		int new_index = current_index + direction;
		
		// Clamp: Keep within bounds
		if (new_index < 0) {
			new_index = 0;  // Stay at the first element
		}
		else if (new_index >= static_cast<int>(m_list_items.size())) {
			new_index = static_cast<int>(m_list_items.size()) - 1;  // Stay at the last element
		}
		
		selectItem(m_list_items[new_index]);	
	}
}