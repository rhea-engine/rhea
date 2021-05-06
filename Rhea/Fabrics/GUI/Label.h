#ifndef RHEA_GUI_LABEL
#define RHEA_GUI_LABEL

#include "Widget.h"

namespace Rhea
{
	class TextWidget;

	class Label : public Widget {

	public:
		Label(std::string path_name) :
			Label(path_name, "//meworla/gui/label")
		{}
		Label(std::string path_name, std::string type_name) :
			Widget(path_name, type_name),			
			m_label_item(nullptr)
		{}

		virtual void setup(Application* app, std::unordered_map<std::string, std::string> attributes, std::unordered_map<std::string, Variant> values);

		static Label* createType(std::string name, std::string type_name);

		// update the text
		void setText(std::string text);

		// events

		void setArea(bool moved, bool sized);

		void ready();

	protected:

		TextWidget* m_label_item;
	};
}

#endif // !RHEA_GUI_LABEL
