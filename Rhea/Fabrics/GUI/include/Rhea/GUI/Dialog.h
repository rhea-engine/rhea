#ifndef RHEA_GUI_DIALOG_H
#define RHEA_GUI_DIALOG_H

#include "Widget.h"
#include "Label.h"

namespace rhea
{
	class Dialog : public Widget
	{
		MECLASS(Dialog, Widget)
	public:
		Dialog(std::string path_name) :
			Dialog(path_name, "//meworla/gui/dialog")
		{}
		Dialog(std::string path_name, std::string type_name) :
			Widget(path_name, type_name)		
		{}

		void ready();		

		void setup(Application* app, std::unordered_map<std::string, std::string> attributes, std::unordered_map<std::string, Variant> values);

		void onSetPosition();

		// styling
		const std::string getSelector() { return "Dialog"; }

	protected:
		
	};
}

#endif // !RHEA_GUI_DIALOG_H
