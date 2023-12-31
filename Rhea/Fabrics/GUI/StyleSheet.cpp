#include "StyleSheet.h"
#include "Application.h"

#include <algorithm>

namespace Rhea
{
	void StyleSheet::setup(Application* app, std::unordered_map<std::string, std::string> attributes, std::unordered_map<std::string, Variant> values)
	{
		Unit::setup(app, attributes, values);
		refs = Unit::getReferences();
	}

	std::vector<Unit::Export> StyleSheet::getReferences()
	{
		if (m_values.find("path") != m_values.end())
			refs.push_back({ "path", "text/css", m_values["path"] });
		return refs;
	}

	void StyleSheet::applyStyle(Widget* w)
	{
		for (auto it = m_entries.begin(); it != m_entries.end(); ++it) {
			
			std::string type = (*it).selector.name;
			std::transform(type.begin(), type.end(), type.begin(), ::tolower);
			if (!std::string("//meworla/gui/" + type).compare(w->getTypeName())) {
				w->setStyle((*it).properties);
			}
		}
	}

	void StyleSheet::setStyle(std::vector<SimpleParser::CssParser::Entry>& entries)
	{
		m_entries = entries;
		int i = 0;
		for (auto it = m_entries.begin(); it != m_entries.end(); ++it)
			for (auto p_it = (*it).properties.begin(); p_it != (*it).properties.end(); ++p_it)
			{
				if (!p_it->first.compare("background-image")) {
					refs.push_back({ "image_" + std::to_string(i++), "image/png", p_it->second[0] });
				}
			}
	}
}

