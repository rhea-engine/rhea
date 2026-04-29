/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#ifndef RHEA_GUI_STYLEENGINE_H
#define RHEA_GUI_STYLEENGINE_H

#include <vector>
#include <unordered_map>
#include <string>

//#include "../Widget.h"
//#include "StyleSheet.h"
#include "Property.h"
#include <SimpleParser/Variant.h>
#include "StyleRule.h"

namespace rhea
{
	class StyleSheet;
	class Widget;

	class StyleEngine
	{
	public:
		void AppendStylesheet(StyleSheet* sheet);
		CssRuleSet GetStylesForWidget(Widget* widget);
		bool HasPseudoClassStyles(Widget* widget, const std::string& pseudo_class) const;
		
	protected:
		struct SelectorKey {
			std::string base_selector;
			std::string pseudo_class;
			
			bool operator==(const SelectorKey& other) const {
				return base_selector == other.base_selector && pseudo_class == other.pseudo_class;
			}
		};

		struct SelectorKeyHash {
			std::size_t operator()(const SelectorKey& key) const {
				return std::hash<std::string>()(key.base_selector) ^ 
					(std::hash<std::string>()(key.pseudo_class) << 1);
			}
		};

		std::vector<StyleSheet*> m_stylesheets;
		std::unordered_map<SelectorKey, CssRuleSet, SelectorKeyHash> m_class_rules;
		std::unordered_map<SelectorKey, CssRuleSet, SelectorKeyHash> m_type_rules;
		std::unordered_map<SelectorKey, CssRuleSet, SelectorKeyHash> m_name_rules;
		
		bool matchesPseudoClass(const std::string& pseudo, Widget* widget) const;
	};
}

#endif // !RHEA_GUI_STYLEENGINE_H
