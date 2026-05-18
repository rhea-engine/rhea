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
		m_values["overflow"] = getValue("overflow", "hidden");
		m_scroll_distance = getValue("scroll_distance", 20.0f); // should this be font - size ?
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

	}
	void ScrollView::onSetPosition()
	{
		m_max_scroll_y = std::max(0.0f, m_child_content_size.y - m_content_size.y);
		m_scroll_y = std::clamp(m_scroll_y, 0.0f, m_max_scroll_y);

		m_content->setPosition(m_position + m_content_position + Vector2(0.0f, -m_scroll_y));
	}

	Vector2 ScrollView::innerSize(Vector2 constraints)
	{
		if (!m_content)
			return Vector2(0, 0);

		Vector2 content_size = m_content->size(Vector2(constraints.x, 999999.0f));

		m_child_content_size = content_size;

		return Vector2(
			content_size.x,
			std::min(content_size.y, constraints.y)
		);
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