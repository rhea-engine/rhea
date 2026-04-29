/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#include <Rhea/GUI/Styles/StyleSheet.h>
#include <Rhea/Application.h>

#include <algorithm>
#include <Rhea/GUI/Styles/CssShorthand.h>

namespace rhea
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

	void StyleSheet::setStyle(std::vector<simpleparser::CssParser::Entry>& entries)
	{
		int i = 0;
		for (auto it = entries.begin(); it != entries.end(); ++it) {
			StyleRule rule;
			rule.selector = (*it).selector;
			for (auto p_it = (*it).properties.begin(); p_it != (*it).properties.end(); ++p_it)
			{
				const CssProperty* prop = CssProperty::FindCssProperty(p_it->first);
				if (prop) {
					// parse the values
					std::vector<CssValue> values;
					ParseValues(prop->id, p_it->second, values);

					if (prop->shorthand) {
						const CssShorthand* s = static_cast<const CssShorthand*>(prop);
						if (s->Parse(values, [&rule](StylePropertyId id, std::vector<CssValue>& value)->void {
							rule.properties[id] = value;
							})) 
						{}
					}
					else {
						rule.properties[prop->id] = values;						

						if (prop->id == StylePropertyId::kBackgroundImage) {
							if (values[0].value != "none")
								refs.push_back({ "image_" + std::to_string(i++), "image/png", values[0].value });
						}
					}
				}
			}
			if (rule.properties.size())
				m_entries.push_back(rule);
		}
	}
	bool StyleSheet::ParseValues(StylePropertyId id, std::vector<simpleparser::CssParser::Property>& content, std::vector<CssValue>& values)
	{
		int state = 0;
		for (int i = 0; i < content.size(); i++)
		{
			switch (state) {
			case 0:
				// get value as string
				std::string value_s = content[i].value.toString();
				switch (content[i].id) {
				case simpleparser::CssParser::PropertyId::kString:
					values.push_back({ CssValueId::kString, content[i].value });
					break;
				case simpleparser::CssParser::PropertyId::kNumeric:
					values.push_back({ CssValueId::kNumeric, content[i].value });
					break;
				case simpleparser::CssParser::PropertyId::kFunction:
					if (!value_s.compare("url")) {
						int anchestor = i;
						while (i + 1 < content.size() && content[i+1].anchestor == anchestor) {
							i++;
							if (content[i].id == simpleparser::CssParser::PropertyId::kString) {
								values.push_back({ CssValueId::kURI, content[i].value });
							}							
						}
					}
					break;
				case simpleparser::CssParser::PropertyId::kIdentifier:
					values.push_back({ CssValueId::kKeyword, content[i].value });
					break;
				case simpleparser::CssParser::PropertyId::kDimension:
					if (content[i].extra=="px")
						values.push_back({ CssValueId::kNumeric, content[i].value });
					else if (content[i].extra=="%")
						values.push_back({ CssValueId::kPercentage, content[i].value });
					else if (content[i].extra == "em")
						values.push_back({ CssValueId::kDimensionEm, content[i].value });
					break;
				}
				break;
			}
		}
		return true;
	}
}

