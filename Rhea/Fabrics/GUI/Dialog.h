#ifndef RHEA_GUI_DIALOG_H
#define RHEA_GUI_DIALOG_H

#include "Widget.h"
#include "Label.h"

namespace Rhea
{
	class Dialog : public Widget
	{
	public:
		Dialog(std::string path_name) :
			Dialog(path_name, "//meworla/gui/dialog")
		{}
		Dialog(std::string path_name, std::string type_name) :
			Widget(path_name, type_name)		
		{}

		void setup(Application* app, std::unordered_map<std::string, std::string> attributes, std::unordered_map<std::string, Variant> values);

		static Dialog* createType(std::string name, std::string type_name);

	protected:
		
	};
}

#endif // !RHEA_GUI_DIALOG_H
