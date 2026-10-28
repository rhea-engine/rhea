/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#include <Rhea/GUI/Row.h>

void rhea::RowWidget::ready()
{
	Widget::ready();

	if (!m_alignment)
		m_alignment = std::make_unique<LayoutAlignment>(LayoutAlignment::JustifyContent::START, Alignment::AlignItems::START);
}

void rhea::RowWidget::onSetPosition()
{
	Vector2 cur_position = m_position + m_content_position;
	if (!m_ordered_children.size())
		return;
	float slot_padding = 0.0f;

	std::vector<Widget*> widgets;
	for (auto it = m_ordered_children.begin(); it != m_ordered_children.end(); ++it) {
		Widget* w = doCast<Widget>(*it);
		if (w && w->isVisible()) {
			widgets.push_back(w);
		}
	}

	AxisLayout layout = m_alignment->justify(m_content_size.x, m_children_size, widgets.size());

	cur_position.x += layout.offset;

	// set fallback slot size to content height
	Vector2 slot_size = Vector2(0.0f, m_content_size.y);

	for (Widget* w : widgets) {

		// get size of child
		Vector2 child_outer_size = w->getOuterSize();
		if (child_outer_size.x == 0.0f)
			child_outer_size.x = w->W();

		slot_size.x = child_outer_size.x;

		// apply alignment
		Vector2 child_position = cur_position;

		child_position.y += m_alignment->align(m_content_size.y, child_outer_size.y);

		w->setPosition(child_position);

		// advance to next slot
		cur_position.x += child_outer_size.x + m_alignment->getGap() + layout.spacing;
	}
}

Vector2 rhea::RowWidget::innerSize(Vector2 size_constraints)
{
	Vector2 inner_size(0.0f, 0.0f);
	const float gap = m_alignment ? m_alignment->getGap() : 0.0f;
	int child_count = 0;

	for (auto* child : m_ordered_children) {
		Widget* widget = doCast<Widget>(child);
		if (!widget || !widget->isVisible())
			continue;

		if (child_count > 0) {
			inner_size.x += gap;
			size_constraints.x -= gap;
		}

		Vector2 child_size = widget->size(size_constraints);
		if (child_size.y > inner_size.y)
			inner_size.y = child_size.y;

		inner_size.x += child_size.x;
		size_constraints.x -= child_size.x;
		++child_count;
	}

	//store for layout
	m_children_size = inner_size.x;

	return inner_size;
}

void rhea::RowWidget::innerGetMinSize(Vector2 size_constraints, Vector2& min_size)
{
	Vector2 inner_min(0.0f, 0.0f);
	const float gap = m_alignment ? m_alignment->getGap() : 0.0f;
	int child_count = 0;

	for (auto* child : m_ordered_children) {
		Widget* widget = doCast<Widget>(child);
		if (!widget	|| !widget->isVisible())
			continue;

		if (child_count > 0)
			inner_min.x += gap;

		Vector2 child_min = widget->getMinSize(size_constraints);
		if (child_min.y > inner_min.y)
			inner_min.y = child_min.y;

		inner_min.x += child_min.x;
		++child_count;
	}

	if (min_size.x < inner_min.x)
		min_size.x = inner_min.x;
	if (min_size.y < inner_min.y)
		min_size.y = inner_min.y;
}
