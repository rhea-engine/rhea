#ifndef RHEA_GUI_STYLE_RULE_H
#define RHEA_GUI_STYLE_RULE_H

#include <string>
#include <unordered_map>
#include "Property.h"

namespace Rhea
{
	struct StyleRule
	{
		// For simplicity, the selector here is assumed to be a class name,
		// optionally prefixed with a dot (e.g., ".button").
		SimpleParser::CssParser::Selector selector;
		// A mapping of style properties (e.g., "background-color") to their values.
		std::unordered_map<StylePropertyID, SimpleParser::Variant> properties;
	};
}

#endif // !RHEA_GUI_STYLE_RULE_H
