/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#ifndef RHEA_GUI_STYLESHEET_H
#define RHEA_GUI_STYLESHEET_H

#include <Rhea/Resources/Resource.h>
#include <SimpleParser/CssParser.h>
#include "../Widget.h"
#include "StyleRule.h"

namespace rhea
{
	class StyleSheet : public Unit
	{
		RECLASS(StyleSheet, Unit, "//meworla/gui/stylesheet")
	public:
		StyleSheet(std::string path_name, std::string type_name) :
			Unit(path_name, type_name)
		{}
		void setup(Application* app, std::unordered_map<std::string, std::string> attributes, std::unordered_map<std::string, Variant> values);

		std::vector<Unit::ResourceReference> getReferences();

		void setStyle(std::vector<simpleparser::CssParser::Entry>& entries);

		std::vector<StyleRule>& getRules() { return m_entries; }

		static bool ParseValues(StylePropertyId id, std::vector<simpleparser::CssParser::Property>& content, std::vector<CssValue>& values);

	protected:
		std::vector<StyleRule> m_entries;
		std::vector<Unit::ResourceReference> refs;
	};
}

#endif // !RHEA_GUI_STYLESHEET_H
