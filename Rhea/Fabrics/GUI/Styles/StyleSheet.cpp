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

	/*void StyleSheet::applyStyle(Widget* w)
	{
		for (auto it = m_entries.begin(); it != m_entries.end(); ++it) {
			
			std::string type_name = (*it).selector;
			std::transform(type_name.begin(), type_name.end(), type_name.begin(), ::tolower);
			SimpleParser::Type* type = SimpleParser::Type::get(std::string("//meworla/gui/" + type_name));
			if (type && w->getType()==type) {			
				w->setStyle((*it).properties);
			}
		}
	}
	*/

	void StyleSheet::setStyle(std::vector<SimpleParser::CssParser::Entry>& entries)
	{
		int i = 0;
		for (auto it = entries.begin(); it != entries.end(); ++it) {
			StyleRule rule;
			rule.selector = (*it).selector.name;
			for (auto p_it = (*it).properties.begin(); p_it != (*it).properties.end(); ++p_it)
			{
				if (!p_it->first.compare("background-image")) {
					refs.push_back({ "image_" + std::to_string(i++), "image/png", p_it->second[0] });
					const CssProperty* prop = CssProperty::FindCssProperty("background-image");
					if (prop)
						rule.properties[prop->id] = p_it->second[0];
				}
			}
			if (rule.properties.size())
				m_entries.push_back(rule);
		}
	}
}

