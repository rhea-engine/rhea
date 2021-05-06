#include "Label.h"
#include "Application.h"
#include "UnitTree.h"
#include "Scene.h"
#include "../Text/TextWidget.h"

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
		val["text"] = getValue("text");
		val["font"] = getValue("font");
		if (val["font"] == Variant()) {
			// get font from gui
		}
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
			if (moved)
				m_label_item->move((int)m_screen_position.x, (int)m_screen_position.y);
			if (sized)
				m_label_item->resize((int)m_size.x, (int)m_size.y);
		}
	}

	void Label::ready()
	{
		if (m_parent) {

		}

		// find the scene
		Scene* scene = (Scene*)m_app->getUnitTree()->findByType("//meworla/scene");
		if (!scene) {
			m_app->log("Widget: Scene not found");
			return;
		}

		if (m_label_item) {
			m_label_item->ready();
			scene->addItem((Item*)m_label_item);
		}

		// handle children
		Unit::ready();
	}
}