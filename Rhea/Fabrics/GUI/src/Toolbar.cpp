/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */
#define NOMINMAX
#include <Rhea/GUI/Toolbar.h>
#include <algorithm>

namespace rhea
{
	void Toolbar::ready()
	{
		Widget::ready();
	}

	void Toolbar::onSetPosition()
	{
		Vector2 cur_position = m_position + m_content_position;

		m_start_container->setPosition(cur_position);
		m_fill_container->setPosition({ cur_position.x + m_start_width + m_spacing,  cur_position.y });		
		m_end_container->setPosition({ cur_position.x + m_content_size.x - m_end_width, cur_position.y});
	}

	math::Vector2 Toolbar::innerSize(math::Vector2 constraints)
	{
		if (!m_start_container || !m_fill_container || !m_end_container)
			return { 0.0f, 0.0f };
		
		float available_width = constraints.x;

		math::Vector2 start_size = m_start_container->size({ available_width, constraints.y });
		math::Vector2 end_size = m_end_container->size({ std::max(0.0f, available_width - start_size.x - m_spacing), constraints.y });

		m_start_width = start_size.x;
		m_end_width = end_size.x;
		m_fill_width = std::max(0.0f, available_width - start_size.x - end_size.x - 2 * m_spacing);

		Vector2 fill_size = m_fill_container->size({m_fill_width, constraints.y});
		float heigth = std::max({ start_size.y, end_size.y, fill_size.y });
		return { available_width, heigth };
	}

	void Toolbar::addStart(rhea::Widget* widget)
	{
		if (!widget)
			return;
		if (assignToSlot("start", widget))
			invalidateLayout();
	}

	void Toolbar::addFill(rhea::Widget* widget)
	{
		if (!widget)
			return;
		if (assignToSlot("fill", widget))
			invalidateLayout();
	}

	void Toolbar::addEnd(rhea::Widget* widget)
	{
		if (!widget)
			return;
		if (assignToSlot("end", widget))
			invalidateLayout();
	}

	bool Toolbar::addToSlot(std::string slot, Widget* child)
	{
		if (slot == "start" && m_start_container)
			return m_start_container->addChild(child);

		if (slot == "fill" && m_fill_container)
			return m_fill_container->addChild(child);

		if (slot == "end" && m_end_container)
			return m_end_container->addChild(child);

		// before ready() the containers are not created yet, so we just assign the slot and let ready() handle it
		return child->getParent() == this || addChild(child);
	}

	bool Toolbar::removeFromSlot(std::string slot, Widget* child)
	{
		if (slot == "start" && m_start_container)
			return m_start_container->removeChild(child);
		if (slot == "fill" && m_fill_container)
			return m_fill_container->removeChild(child);
		if (slot == "end" && m_end_container)
			return m_end_container->removeChild(child);
		return Widget::removeFromSlot(slot, child);
	}

	Widget* Toolbar::resolveSlotWidget(std::string_view slot_name, std::string_view content_name) const
	{
		if (slot_name == "start" && m_start_container)
			return doCast<Widget>(m_start_container->getChild(std::string(content_name)));
		if (slot_name == "fill" && m_fill_container)
			return doCast<Widget>(m_fill_container->getChild(std::string(content_name)));
		if (slot_name == "end" && m_end_container)
			return doCast<Widget>(m_end_container->getChild(std::string(content_name)));
		return Widget::resolveSlotWidget(slot_name, content_name);
	}

	bool Toolbar::buildDefaultLayout()
	{
		if (m_start_container &&
			m_fill_container &&
			m_end_container)
			return true;

		m_start_container =	emplaceChild<RowWidget>("_start");
		m_fill_container = emplaceChild<RowWidget>("_fill");
		m_end_container = emplaceChild<RowWidget>("_end");

		return m_start_container && m_fill_container && m_end_container;
	}
}