#include "StyleSheet.h"
#include <Rhea/ResourceList.h>

namespace Rhea
{
	void StyleSheet::setup(Application* app, std::unordered_map<std::string, std::string> attributes, std::unordered_map<std::string, Variant> values)
	{
		Unit::setup(app, attributes, values);
		
		if (m_values["path"] != Variant()) {
			m_resource_list = new ResourceList(m_app);
			m_resource_list->push("stylesheet", m_values["path"], "text/css");
		}		
	}

	void StyleSheet::prepare(Unit::ReadyHandler handler)
	{
		Resource::prepare(handler);
	}

	void StyleSheet::applyStyle(Widget* w)
	{
		for (auto it = m_entries.begin(); it != m_entries.end(); ++it) {
			
			std::string type = (*it).selector.name;
			transform(type.begin(), type.end(), type.begin(), ::tolower);
			if (!std::string("//meworla/gui/" + type).compare(w->getTypeName())) {
				w->setStyle((*it).properties);
			}
		}
	}

	void StyleSheet::setStyle(std::vector<SimpleParser::CssParser::Entry>& entries)
	{
		m_entries = entries;
		int i = 0;
		for (auto it= m_entries.begin(); it!=m_entries.end(); ++it)
			for (auto p_it = (*it).properties.begin(); p_it != (*it).properties.end(); ++p_it)
			{
				if (!p_it->first.compare("background-image")) {
					if (!m_resource_list)
						m_resource_list = new ResourceList(m_app);
					m_resource_list->push("image" + i++, p_it->second[0], "image/png");
				}
			}
	}
	
	StyleSheet* StyleSheet::createType(std::string name, std::string type)
	{
		StyleSheet* stylesheet = new StyleSheet(name, type);
		return stylesheet;
	}
}

