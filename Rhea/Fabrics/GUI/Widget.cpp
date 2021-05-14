#include "Widget.h"
#include "WidgetEvent.h"

#include "Application.h"
#include "UnitTree.h"
#include "Scene.h"

#include "../Canvas/Item2D.h"

namespace Rhea
{
	void Widget::setup(Application* app, std::unordered_map<std::string, std::string> attributes, std::unordered_map<std::string, Variant> values)
	{
		Unit::setup(app, attributes, values);

		m_base_position.x = getValue("x");
		m_base_position.y = getValue("y");
		m_position = m_base_position;
		m_base_size.x = getValue("w");
		m_base_size.y = getValue("h");

		// internal
		m_values["isWidget"] = true;

		std::unordered_map<std::string, std::string> attr;
		std::unordered_map<std::string, Variant> val;
		
		val["texture"] = getValue("texture");
		if (val["texture"] != Variant()) {
			val["x"] = m_base_position.x;
			val["y"] = m_base_position.y;
			val["w"] = m_base_size.x;
			val["h"] = m_base_size.y;
			m_background_item = (Item2D*)m_app->getUnitTree()->createUnit(m_path + "/bgimage", "//meworla/widget", attr, val);
			addChild(m_background_item);
		}
	}
	
	void Widget::prepare(Unit::ReadyHandler handler)
	{
		// call parent asynchronously
		Unit::prepare([&](Error* err) {

			if (err) {
				m_app->log("Unit::prepare failed");
			}

			if (m_background_item) {
				m_background_item->prepare([&](Error* err) {
					handler(err);
					});
			}
			else {
				handler(err);
			}
			});
			
	}

	Widget* Widget::createType(std::string name, std::string type)
	{
		Widget* ret = new Widget(name);
		return ret;
	}
	
	void Widget::ready()
	{
		// find the scene
		Scene* scene = (Scene*)m_app->getUnitTree()->findByType("//meworla/scene");
		if (!scene) {
			m_app->log("Widget: Scene not found");
			return;
		}

		if (m_background_item) {
			m_background_item->ready();
			scene->addItem((Item*)m_background_item);
		}

		// handle children
		Unit::ready();
	}
	
	void Widget::onParentSized(Event* event)
	{
		// remember old size
		Vector2 old_size(m_size);

		// recalculate my size
		m_size = size();
		
		// check if the size changed
		const bool sized = (m_size != old_size);

		if (sized) {
			onSized();
		}

	}

	void Widget::onParentMoved()
	{
		m_screen_position_valid = false;
		Vector2 pos = getScreenPosition();

		setArea(true, false);

		// recalc children
		moveChildren();
	}

	void Widget::onSized()
	{
		sizeChildren();

		setArea(false, true);

		WidgetEvent e("Sized", this);
		sendEvent("Sized", (Event*)&e);
	}

	void Widget::setPosition(Vector2 position)
	{
		// if the position really changes recalculate and send event
		if (position != m_position) {
			m_position = position;
			m_screen_position_valid = false;
			onMoved();
		}
	}

	void Widget::onMoved()
	{
		Vector2 pos = getScreenPosition();

		setArea(true, false);

		// recalc screen coords of children
		sizeChildren();

		WidgetEvent e("Moved", this);
		sendEvent("Moved", (Event*)&e);
	}

	void Widget::sizeChildren()
	{
		for (auto it = m_children.begin(); it != m_children.end(); ++it) {
			Widget* w = (Widget*)it->second;
			if (w->getValue("isWidget", false)) {
				w->onParentSized(nullptr);
			}
		}
	}

	void Widget::moveChildren()
	{
		for (auto it = m_children.begin(); it != m_children.end(); ++it) {
			Widget* w = (Widget*)it->second;
			if (w->getValue("isWidget", false)) {
				w->onParentMoved();
			}
		}
	}

	Vector2 Widget::size()
	{
		m_size = m_base_size;
		if (m_parent && !m_top_window) {
			// get parents area minus my relative position
			Vector2 p_size = m_size = static_cast<Widget*>(m_parent)->getSize() - m_position;
			// clip to parent
			if (p_size.x < m_size.x)
				m_size.x = p_size.x;
			if (m_size.x < 0.0f)
				m_size.x = 0.0f;
			if (p_size.y < m_size.y)
				m_size.y = p_size.y;
			if (m_size.y < 0.0f)
				m_size.y = 0.0f;
		}

		return m_size;
	}
	
	Widget* Widget::capture(int x, int y)
	{
		if (m_screen_position.x > x ||
			m_screen_position.x + m_size.x < x ||
			m_screen_position.y > y ||
			m_screen_position.y + m_size.y < y) {
			return nullptr;
		}

		// is a child a better match?
		Widget* captured;
		for (auto it = m_children.begin(); it != m_children.end(); ++it)
		{
			Widget* w = (Widget*)it->second;
			if (w->getValue("isWidget", false)) {
				captured = w->capture(x, y);
				if (captured) {
					return captured;
				}
			}
		}
		return this;
	}

	Widget* Widget::getInputTarget()
	{
		// if we handle input return myself
		if (m_handle_input)
			return this;

		// if we are not a top window we have a Widget parent - check it.
		if (!m_top_window)
			return static_cast<Widget*>(m_parent)->getInputTarget();

		return nullptr;
	}

	Vector2 Widget::getScreenPosition()
	{
		if (!m_screen_position_valid) {
			if (m_top_window) {
				m_screen_position = m_position;
			}
			else {
				m_screen_position = static_cast<Widget*>(m_parent)->getScreenPosition() + m_position;
			}
			m_screen_position_valid = true;
		}
		return m_screen_position;
	}
	
	void Widget::setArea(bool moved, bool sized)
	{
		// update my background image item
		if (m_background_item) {
			if (moved)
				m_background_item->move((int)m_position.x, (int)m_position.y);
			if (sized)
				m_background_item->resize((int)m_size.x, (int)m_size.y);
		}
	}
}