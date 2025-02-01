#ifndef RHEA_GUI_STYLEENGINE_H
#define RHEA_GUI_STYLEENGINE_H

#include <vector>
#include <unordered_map>
#include <string>

//#include "../Widget.h"
//#include "StyleSheet.h"
#include "Property.h"

namespace Rhea
{
	class StyleSheet;
	class Widget;

	class StyleEngine
	{
	public:
		void AppendStylesheet(StyleSheet* sheet);

		std::unordered_map<StylePropertyID, std::string> GetStylesForWidget(Widget* widget);

	protected:
		// Holds pointers to appended style sheets.
		std::vector<StyleSheet*> styleSheets;
	};
}

#endif // !RHEA_GUI_STYLEENGINE_H
