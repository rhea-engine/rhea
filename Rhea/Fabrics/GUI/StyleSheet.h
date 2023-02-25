#ifndef RHEA_GUI_STYLESHEET_H
#define RHEA_GUI_STYLESHEET_H

#include <Rhea/Resource.h>
#include <SimpleParser/CssParser.h>
#include "Widget.h"

namespace Rhea
{
	class StyleSheet : public Resource
	{
	public:
		StyleSheet(std::string path_name) :
			StyleSheet(path_name, "//meworla/gui/stylesheet")
		{}
		StyleSheet(std::string path_name, std::string type_name) :
			Resource(path_name, type_name)
		{}
		void setup(Application* app, std::unordered_map<std::string, std::string> attributes, std::unordered_map<std::string, Variant> values);

		void prepare(Unit::ReadyHandler handler);

		void applyStyle(Widget* w);

		void setStyle(std::vector<SimpleParser::CssParser::Entry>& entries);

		// type functions
		static StyleSheet* createType(std::string name, std::string type);

	protected:
		std::vector<SimpleParser::CssParser::Entry> m_entries;
	};
}

#endif // !RHEA_GUI_STYLESHEET_H
