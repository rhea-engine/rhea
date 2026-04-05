#ifndef RHEA_GUI_STYLE_RULE_H
#define RHEA_GUI_STYLE_RULE_H

#include <string>
#include <unordered_map>
#include <SimpleParser/CssParser.h>
#include "Property.h"
#include "CssValue.h"

namespace rhea
{
	typedef std::unordered_map<StylePropertyId, std::vector<CssValue>> CssRuleSet;

	struct StyleRule
	{
		// For simplicity, the selector here is assumed to be a class name,
		// optionally prefixed with a dot (e.g., ".button").
		simpleparser::CssParser::Selector selector;
		// A mapping of style properties (e.g., "background-color") to their values.
		CssRuleSet properties;
	};
}

#endif // !RHEA_GUI_STYLE_RULE_H
