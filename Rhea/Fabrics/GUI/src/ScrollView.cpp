#define NOMINMAX
#include <Rhea/GUI/ScrollView.h>
#include <algorithm>
#include <Rhea/EventInput.h>
#include <Rhea/Input/InputEvent.h>

namespace rhea
{
	void ScrollView::setup(Application* app, std::unordered_map<std::string, std::string> attributes, std::unordered_map<std::string, Variant> values)
	{
		Widget::setup(app, attributes, values);
		m_values["overflow"] = getValue("overflow", "scroll");
		m_values["show_scrollbar"] = getValue("show_scrollbar", true);
		setStyle(StylePropertyId::kOverflow, m_values["overflow"].toString());
		m_scroll_distance = getValue("scroll_distance", 20.0f); // should this be font - size ?
		m_scrollbar_width = getValue("scrollbar_width", m_scrollbar_width);
	}

	void ScrollView::ready()
	{
		Widget::ready();

		// get content widget
		for (auto& child : m_children) {
			Widget* widget_child = doCast<Widget>(child.second);
			if (widget_child) {
				m_content = widget_child;
				break;
			}
		}	

		// Create scrollbar if enabled
		if (getValue("show_scrollbar", true)) {
			m_scrollbar = simpleparser::Type::create<ScrollBar>(getPathName() + "_scrollbar");
			m_scrollbar->setup(m_app, {}, {
				{"orientation", "vertical"}
				});
			addChild(m_scrollbar);
			m_scrollbar->ready();

			// Connect scrollbar events to scroll position
			m_scrollbar->setOnScroll([this](float position) {
				float abs_position = position * m_max_scroll_y;
				setScrollY(abs_position);
				});
		}

	}
	void ScrollView::onSetPosition()
	{
		m_max_scroll_y = std::max(0.0f, m_child_content_size.y - m_content_size.y);
		m_scroll_y = std::clamp(m_scroll_y, 0.0f, m_max_scroll_y);

		m_content->setPosition(m_position + m_content_position + Vector2(0.0f, -m_scroll_y));

		// Update scrollbar thumb position
		if (m_scrollbar) {
			float thumb_ratio = (m_child_content_size.y > 0.0f)
				? std::clamp(m_content_size.y / m_child_content_size.y, 0.0f, 1.0f)
				: 1.0f;

			float thumb_pos = (m_max_scroll_y > 0.0f)
				? (m_scroll_y / m_max_scroll_y) * (1.0f - thumb_ratio)
				: 0.0f;
			m_scrollbar->setThumbPosition(thumb_pos, thumb_ratio);
			m_scrollbar->setPosition(Vector2(m_position.x + m_content_size.x - m_scrollbar_width, m_position.y));
		}
	}

	Vector2 ScrollView::innerSize(Vector2 constraints)
	{
		if (!m_content)
			return Vector2(0, 0);

		// Reserve space for scrollbar
		float scrollbar_width = (m_scrollbar != nullptr) ? m_scrollbar_width : 0.0f;
		
		Vector2 content_size = m_content->size(Vector2(constraints.x - scrollbar_width, 999999.0f));
		//Vector2 natural_size = m_content->innerSize(content_size);

		m_child_content_size = content_size;

		if (m_scrollbar) {
			m_scrollbar->size(Vector2(m_scrollbar_width, constraints.y));
		}

		return Vector2(
			constraints.x,
			std::min(content_size.y, constraints.y)
		);
	}

	void ScrollView::invalidateLayout()
	{
		if (m_layout_dirty)
			return;

		m_layout_dirty = true;

		static bool inInvalidateLayout = false;

		if (inInvalidateLayout) {
			return;
		}
		inInvalidateLayout = true;

		// also invalidate scrollbar if present
		if (m_scrollbar)
			m_scrollbar->invalidateLayout();

		if (auto* p = doCast<Widget>(m_parent))
			p->invalidateLayout();

		inInvalidateLayout = false;
	}

	bool ScrollView::onMouseWheel(Event* ev, Unit* unit)
	{
		ev->stopPropagation();
		//if (m_values["overflow"].toString().compare("hidden") == 0)
		//		return false;
		float wheelY = static_cast<EventInput*>(ev)->getEvent().wheelY;
		setScrollY(m_scroll_y - wheelY / 120.0f * m_scroll_distance); // note: 120.0f should be "one line" turn.
		return false;
	}

	void ScrollView::setScrollY(float y)
	{
		m_scroll_y = std::clamp(y, 0.0f, m_max_scroll_y);
		invalidateLayout();
	}

	void ScrollView::ensureVisible(Widget* child)
	{
		float view_top = m_position.y;
		float view_bottom = m_position.y + H();

		float child_top = child->Y();
		float child_bottom = child->Y() + child->H();

		if (child_top < view_top) {
			setScrollY(m_scroll_y - (view_top - child_top));
		}
		else if (child_bottom > view_bottom) {
			setScrollY(m_scroll_y + (child_bottom - view_bottom));
		}
	}
}