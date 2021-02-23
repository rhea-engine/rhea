#ifndef RHEA_TEXTWIDGET_H
#define RHEA_TEXTWIDGET_H

#include "Resource.h"
#include "Material.h"
#include "Font.h"
#include "../Canvas/Item2D.h"

namespace Rhea
{
	class TextWidget : public Item2D
	{
	public:
		TextWidget(std::string name) :
			TextWidget(name, "//meworla/textwidget")
		{}
		TextWidget(std::string name, std::string type_name) :
			m_font(nullptr),
			Item2D(name, type_name)
		{}

		// setup from strings
		void setup(Application* app, std::unordered_map<std::string, std::string> attributes, std::unordered_map<std::string, Variant> values);

		// ready event handler
		void ready();

		// get the Text
		std::string getText() { return m_text; }

		// get the font
		Font* getFont() { return m_font; }

		// factory function
		static Unit* createType(std::string name, std::string type);

	protected:
		std::string m_text;
		Font* m_font;
	};
}

#endif // !RHEA_TEXTWIDGET_H
