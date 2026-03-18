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

namespace Rhea
{
	class StyleSheet;
	class Widget;

	class StyleEngine
	{
	public:
		void AppendStylesheet(StyleSheet* sheet);

		CssRuleSet GetStylesForWidget(Widget* widget);

	protected:
		// Holds pointers to appended style sheets.
		std::vector<StyleSheet*> m_stylesheets;
		std::unordered_map<std::string, CssRuleSet> m_class_rules;
		std::unordered_map<std::string, CssRuleSet> m_type_rules;
		std::unordered_map<std::string, CssRuleSet> m_name_rules;
	};
}

#endif // !RHEA_GUI_STYLEENGINE_H
