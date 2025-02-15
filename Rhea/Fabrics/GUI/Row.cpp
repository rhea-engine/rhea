#include "Row.h"

void Rhea::RowWidget::ready()
{
	Widget::ready();

	if (!m_alignment)
		m_alignment = std::make_unique<Alignment>(Alignment::Horizontal::CENTER, Alignment::Vertical::CENTER);
}

void Rhea::RowWidget::onSetPosition()
{
	Vector2 cur_position = m_position + m_content_position;
	if (!m_ordered_children.size())
		return;
	float slot_padding;

	slot_padding = (m_content_size.x - m_children_size) / m_ordered_children.size();
	Vector2 slot_size = Vector2(0.0f, m_content_size.y);

	for (auto it = m_ordered_children.begin(); it != m_ordered_children.end(); ++it) {
		Widget* w = doCast<Widget>(*it);
		if (w) {
			// get size of child
			Vector2 child_outer_size = w->getOuterSize();
			if (child_outer_size.x == 0.0f)
				child_outer_size.x = w->W();

			float slot_width = child_outer_size.x + slot_padding;
			slot_size.x = child_outer_size.x + slot_padding;
			
			// apply alignment
			Vector2 child_position = cur_position;

			if (m_alignment)
				m_alignment->apply(slot_size, child_outer_size, child_position);

			w->setPosition(child_position);

			// advance to next slot
			cur_position.x += slot_width;
		}
	}
}

Vector2 Rhea::RowWidget::innerSize(Vector2 size_constraints)
{
	Vector2 innerSize(0.0f, 0.0f);

	// TODO: check for scrolling
	// TODO Use min size for enlarging children
	//Vector2 min_size = getMinSize(size_constraints);

	for (auto it = m_ordered_children.begin(); it != m_ordered_children.end(); ++it) {
		Widget* w = doCast<Widget>(*it);
		if (w) {
			Vector2 child_size = w->size(size_constraints);
			if (child_size.y > innerSize.y)
				innerSize.y = child_size.y;
			innerSize.x += child_size.x;
			size_constraints.x -= child_size.x;
		}
	}
	//store for layout
	m_children_size = innerSize.x;

	return innerSize;
}

void Rhea::RowWidget::innerGetMinSize(Vector2 size_constraints, Vector2& min_size)
{
	Vector2 inner_min;

	for (auto it = m_children.begin(); it != m_children.end(); ++it) {
		Widget* w = doCast<Widget>(it->second);
		if (w) {
			Vector2 child_min = w->getMinSize(size_constraints);
			if (child_min.y > inner_min.y)
				inner_min.y = child_min.y;
			inner_min.x += child_min.x;
		}
	}
	if (min_size.x < inner_min.x)
		min_size.x = inner_min.x;
	if (min_size.y < inner_min.y)
		min_size.y = inner_min.y;

}
