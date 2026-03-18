#include <Rhea/GUI/List.h>
#include <Rhea/GUI/ListItem.h>
#include <Rhea/Application.h>
#include <Rhea/UnitTree.h>

namespace Rhea
{
	void Rhea::ListWidget::setup(Application* app, std::unordered_map<std::string, std::string> attributes, std::unordered_map<std::string, Variant> values)
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

		Event e("scene_enter", item);
		bubbleEvent("scene_enter", &e);
		Event e2("need_resize", item);
		bubbleEvent("need_resize", &e2);
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
		for (ListWidgetItem* w : to_del) {
			removeChild(w);
			Event e("scene_leave", w);
			bubbleEvent("scene_leave", &e);
			delete w;
		}
		// clear list
		m_list_items.clear();
	}

	void ListWidget::ready()
	{
		Widget::ready();

		// check if we have a data binding
		if (m_values["data"] != Variant()) {
			m_data_binding = m_app->getUnitTree()->getBinding(m_values["data"], m_attributes["basepath"]);

			// fill the list once
			if (m_data_binding.target) {
				pollData();
			}
		}

		registerEventHandler("selected", this, std::bind(&ListWidget::onSelected, this, std::placeholders::_1, std::placeholders::_2));
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

			if (m_list_items.size()) {
				if (m_list_items.size() == items.length()) {
					for (int i = 0; i < items.length(); i++) {
						if (std::string(m_list_items[i]->getValue("value")).compare(items[i]))
							changed = true;
					}
				}
				else
					changed = true;
				if (changed) {

					// iterate over children and delete all listwidgetitems
					std::vector<ListWidgetItem*> to_del;
					for (auto it = m_children.begin(); it != m_children.end(); ++it) {
						ListWidgetItem* w = doCast<ListWidgetItem>(it->second);
						if (w) {
							to_del.push_back(w);
						}
					}
					for (ListWidgetItem* w : to_del) {
						removeChild(w);
						delete w;
					}
					// clear list
					m_list_items.clear();					
				}
			}
			else
				changed = true;
			if (changed) {
				for (int i = 0; i < items.length(); i++) {
					Variant item = items[i];
					addItem(item, item);
				}
			}
		}
	}

	bool ListWidget::onSelected(Event* ev, Unit* unit)
	{
		if (m_values["onSelected"] != Variant()) {
			sendBoundEvent(m_values["onSelected"], ev);
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
}