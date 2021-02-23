#include "TextWidget.h"
#include "Application.h"
#include "ImageResource.h"
#include "TextResource.h"
#include "UnitTree.h"
#include "ResourceList.h"

namespace Rhea
{
	void TextWidget::setup(Application* app, std::unordered_map<std::string, std::string> attributes, std::unordered_map<std::string, Variant> values)
	{
		Item2D::setup(app, attributes, values);
		
		m_text = values["text"];

		if (m_values.find("font") != m_values.end()) {
			m_resource_list->push("font", m_values["font"], "//meworla/bitmapfont");
		}
	}

	Unit* TextWidget::createType(std::string name, std::string type)
	{
		Unit* ret = (Unit*)new TextWidget(name, type);
		return ret;
	}

	void TextWidget::ready()
	{
		if (m_status >= Status::READY)
			return;
		m_status = Status::READY;
		Unit::ready();

		// load and add textures
		if (m_values.find("font") != m_values.end())
		{
			m_font = static_cast<Font*>(m_app->getUnitTree()->find(m_values["font"]));
			if (m_font) {
				m_font->ready();
			}
			else {
				m_app->log("TextWidget: Font '" + (std::string)m_values["font"] + "' not found");
			}
		}
	}
}
