#ifndef RHEA_GUI_STYLESHEET_H
#define RHEA_GUI_STYLESHEET_H

#include <Resources/Resource.h>
#include <SimpleParser/CssParser.h>
#include "../Widget.h"
#include "StyleRule.h"

namespace Rhea
{
	class StyleSheet : public Unit
	{
		RECLASS(StyleSheet, Unit, "//meworla/gui/stylesheet")
	public:
		StyleSheet(std::string path_name, std::string type_name) :
			Unit(path_name, type_name)
		{}
		void setup(Application* app, std::unordered_map<std::string, std::string> attributes, std::unordered_map<std::string, Variant> values);

		std::vector<Unit::Export> getReferences();

		//void applyStyle(Widget* w);

		void setStyle(std::vector<SimpleParser::CssParser::Entry>& entries);

		std::vector<StyleRule>& getRules() { return m_entries; }

	protected:
		std::vector<StyleRule> m_entries;
		std::vector<Unit::Export> refs;
	};
}

#endif // !RHEA_GUI_STYLESHEET_H
