#include "List.h"
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

		values["x"] = 5.0f;
		values["y"] = 5.0f + 30.0f * (m_children.size());
		values["w"] = m_size.x - 2 * 5.0f;
		values["h"] = 30.0f - 2 * 5.0f;
		values["text"] = text;
		values["value"] = value;

		Widget* item = (Widget*)m_app->getUnitTree()->createUnit("child_" + std::to_string(1 + m_children.size()), "//meworla/gui/listitem", attributes, values);
		addChild(item);
		item->ready();
		item->onParentSized(nullptr);
		item->onParentMoved();
		Event e("scene_enter", item);
		bubbleEvent("scene_enter", &e);

		registerEventHandler("selected", this, std::bind(&ListWidget::onSelected, this, std::placeholders::_1, std::placeholders::_2));
	}

	bool ListWidget::onSelected(Event* ev, Unit* unit)
	{
		if (m_values["onSelected"] != Variant()) {
			sendBoundEvent(m_values["onSelected"], ev);
		}
		return false;
	}

	ListWidget* Rhea::ListWidget::createType(std::string name, std::string type_name)
	{
		ListWidget* ret = new ListWidget(name);
		return ret;
	}
}