#include "Label.h"
#include "Application.h"
#include "UnitTree.h"
#include "Fabrics/Core/Scene.h"
#include "../Text/TextWidget.h"
#include <Psio/Log.h>

namespace Rhea
{
	void Label::setup(Application* app, std::unordered_map<std::string, std::string> attributes, std::unordered_map<std::string, Variant> values)
	{
		Widget::setup(app, attributes, values);

		std::unordered_map<std::string, std::string> attr;
		std::unordered_map<std::string, Variant> val;

		val["x"] = m_base_position.x;
		val["y"] = m_base_position.y;
		val["w"] = m_base_size.x;
		val["h"] = m_base_size.y;
		val["text"] = getValue("text", "");
		if (m_values.find("font") != m_values.end())
			val["font"] = getValue("font");

		// DEBUG
		// always handle input 
		m_handle_input = true;

		m_label_item = (TextWidget*)m_app->getUnitTree()->createUnit(m_path + "/label", "//meworla/textwidget", attr, val);
		addChild(m_label_item);
	}

	Label* Label::createType(std::string name, std::string type_name)
	{
		Label* ret = new Label(name, type_name);
		return ret;
	}

	void Label::setText(std::string text)
	{
		m_label_item->setText(text);
	}


	void Label::setArea(bool moved, bool sized)
	{
		Widget::setArea(moved, sized);

		if (m_label_item) {			
			if (moved) {
				DLOG(LOG_DEBUG) << "GUI::Label: widget " << m_pathname << ": screen(x,y)=" << m_position.x << ", " << m_position.y << ")";
				m_label_item->move(m_position.x, m_position.y);
			}
			if (sized) {
				DLOG(LOG_DEBUG) << "GUI::Label: widget " << m_pathname << ": size(w,h)" << m_size.x << ", " << m_size.y << ")";
				m_label_item->resize(m_size.x, m_size.y);
			}
			if (sized || moved) {
				m_label_item->update();
			}
		}
	}

	void Label::ready()
	{
		Widget::ready();

		// find the scene
		Scene* scene = (Scene*)m_app->getUnitTree()->findByType("//meworla/scene");
		if (!scene) {
			LOG(LOG_ERROR) << "Widget: Scene not found";
			return;
		}

		if (m_label_item) {
			m_label_item->ready();
			//scene->addItem((Item*)m_label_item);
		}
	}
}