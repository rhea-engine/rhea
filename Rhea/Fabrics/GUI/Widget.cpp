#include "Widget.h"
#include "WidgetEvent.h"

#include "Application.h"
#include "UnitTree.h"
#include "Fabrics/Core/Scene.h"
#include "Fabrics/Media/ImageResource.h"
#include "TextResource.h"

#include "../Canvas/Item2D.h"
#include <Psio/Log.h>

namespace Rhea
{
	void Widget::setup(Application* app, std::unordered_map<std::string, std::string> attributes, std::unordered_map<std::string, Variant> values)
	{
		Item2D::setup(app, attributes, values);

		m_base_position.x = getValue("x");
		m_base_position.y = getValue("y");
		m_cur_position = m_base_position;
		m_base_size.x = getValue("w");
		m_base_size.y = getValue("h");

		// internal
		m_values["isWidget"] = true;

		// set styles based on values
		if (m_values.find("font") != m_values.end())
			m_properties["font"].push_back({ m_values["font"] });
		if (m_values.find("texture") != m_values.end())
			m_properties["background-image"].push_back({ m_values["texture"] });

		std::unordered_map<std::string, std::string> attr;
		std::unordered_map<std::string, Variant> val;		
	}

	Widget* Widget::createType(std::string name, std::string type)
	{
		Widget* ret = new Widget(name);
		return ret;
	}
	
	void Widget::ready()
	{
		// prepare material
		Item2D::ready();

		// find the scene
		Scene* scene = (Scene*)m_app->getUnitTree()->findByType("//meworla/scene");
		if (!scene) {
			LOG(LOG_ERROR) << "Widget: Scene not found";
			return;
		}

		if (m_material) {
		//	scene->addItem(this);
		}
	
		// if we are hidden at the beggining - send the event
		if (!m_visible) {
			hide();
		}
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
		if (position != m_cur_position) {
			m_cur_position = position;
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
		// if size is set, clip to parent
		// if size is not set use remaining size of parent

		m_size = m_base_size;
		if (m_parent && !m_parent->getValue("isWidget", false)) {
			// get remaining parent area (parents area minus my relative position)
			Vector2 p_size = static_cast<Widget*>(m_parent)->getSize() - m_cur_position;

			// get parent remaining size if not set
			if (m_base_size.x == 0)
				m_size.x = p_size.x;
			if (m_base_size.y == 0)
				m_size.y = p_size.y;
			
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

	void Widget::setStyle(SimpleParser::CssParser::PropertyMap& properties)
	{
		for (auto it = properties.begin(); it != properties.end(); ++it) {
			m_properties[it->first] = it->second;
			if (it->first == "background-image") {
				std::string resname = it->second[0];
				if (!resname.compare("none")) {
					if (m_material) {
						// remove background image
						m_material->sendEvent("delete", nullptr);
						delete m_material;
						m_material = nullptr;
						m_render_component->setMaterial(nullptr);
						m_mesh->sendEvent("delete", nullptr);
						delete m_mesh;
						m_mesh = nullptr;
						m_render_component->setMesh(nullptr);
					}
				}
				else if (!m_material) {
					std::unordered_map<std::string, std::string> attr;
					std::unordered_map<std::string, Variant> val;

					
					Unit* tex = m_app->getUnitTree()->find(resname);
					if (tex) {

						// create dummy material
						m_material = new Material(m_path + "_material");
						m_material->setup(m_app);
						m_material->addTexture(static_cast<ImageResource*>(tex));
						m_material->setValue("shader", "widget");

						// get the shader						
						if (m_values.find("shader") == m_values.end()) {
							// set a default
							m_values["shader"] = "//rhea/shaders/widget.hlsl";
						}

						Unit* shader_res = m_app->getUnitTree()->find(m_values["shader"]);
						if (shader_res) {
							m_material->setShader(static_cast<TextResource*>(shader_res));
						}
						else {
							LOG(LOG_ERROR) << "item2d::ready(): Shader '" << (std::string)m_values["shader"] << "' for " << m_pathname << " not found";
						}

						// add the mesh
						if (!m_mesh) {

							std::unordered_map<std::string, std::string> attributes;
							std::unordered_map<std::string, Variant> values;
							m_mesh = static_cast<Mesh*>(m_app->getUnitTree()->createUnit(m_pathname + "_mesh", "//meworla/mesh", attributes, values));
							m_mesh->setMaterial(m_material);

							setMesh();

							m_render_component->setMesh(m_mesh);
						}
						else
							m_mesh->setMaterial(m_material);
						sendEvent("update", nullptr);
					}
				}
				else if (m_values["texture"]==Variant()) {
					DLOG(LOG_DEBUG) << "TODO: Handle owverwriting of texture, if not set on widget";
				}
			}
		}
	}

	std::vector<Variant>* Widget::getStyle(std::string name)
	{
		assert(m_parent);
		auto it = m_properties.find(name);
		if (it != m_properties.end())
			return &it->second;
		else if (m_parent->getValue("isWidget"))
			return ((Widget*)m_parent)->getStyle(name);
		else
			return nullptr;
	}
	
	void Widget::show()
	{
		m_visible = true;
		m_render_component->show();
	}

	void Widget::hide()
	{
		m_visible = false;
		m_render_component->hide();
	}

	Widget* Widget::capture(int c_x, int c_y)
	{
		if (!m_visible)
			return nullptr;

		if (m_position.x > c_x ||
			m_position.x + m_size.x < c_x ||
			m_position.y > c_y ||
			m_position.y + m_size.y < c_y) {
			return nullptr;
		}

		// is a child a better match?
		Widget* captured;
		for (auto it = m_children.begin(); it != m_children.end(); ++it)
		{
			Widget* w = (Widget*)it->second;
			if (w->getValue("isWidget", false)) {
				captured = w->capture(c_x, c_y);
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
				m_position = m_cur_position;
			}
			else {
				m_position = static_cast<Widget*>(m_parent)->getScreenPosition() + m_cur_position;
			}
			m_screen_position_valid = true;
		}
		return m_position;
	}
	
	void Widget::setArea(bool moved, bool sized)
	{
		if (m_material && (sized || moved)) {
			if (moved) {
				DLOG(LOG_DEBUG) << "GUI::Widget: widget " << m_pathname << ": screen(x,y)=" << m_position.x << ", " << m_position.y << ")";			
			}
			if (sized) {
				DLOG(LOG_DEBUG) << "GUI::Widget: widget " << m_pathname << ": size(w,h)" << m_size.x << ", " << m_size.y << ")";
			}
			update();
		} 		
	}
}