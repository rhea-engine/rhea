#ifndef RHEA_GUI_STYLEENGINE_H
#define RHEA_GUI_STYLEENGINE_H

#include <vector>
#include <unordered_map>
#include <string>

//#include "../Widget.h"
//#include "StyleSheet.h"
#include "Property.h"
#include <Components/SimpleParser/Variant.h>

namespace Rhea
{
	class StyleSheet;
	class Widget;

	class StyleEngine
	{
	public:
		void AppendStylesheet(StyleSheet* sheet);

		std::unordered_map<StylePropertyID, SimpleParser::Variant> GetStylesForWidget(Widget* widget);

	protected:
		// Holds pointers to appended style sheets.
		std::vector<StyleSheet*> m_stylesheets;
	};
}

#endif // !RHEA_GUI_STYLEENGINE_H
