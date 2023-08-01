#ifndef RHEA_GUI_TEXTINPUT
#define RHEA_GUI_TEXTINPUT

#include "Widget.h"

namespace Rhea
{
	class TextWidget;
	class Label;

	class TextInput : public Widget {

	public:
		TextInput(std::string path_name) :
			TextInput(path_name, "//meworla/gui/textinput")
		{}
		TextInput(std::string path_name, std::string type_name) :
			Widget(path_name, type_name),
			m_label_item(nullptr),
			m_cursor_item(nullptr),
			m_textwidget(nullptr),
			m_pos(0),
			m_length(0),
			multiline(false),
			m_cursor_shown(false),
			m_cursor_timer(0.0f),
			m_has_focus(false),
			m_offset_x(0.0f)
		{}

		virtual void setup(Application* app, std::unordered_map<std::string, std::string> attributes, std::unordered_map<std::string, Variant> values);

		static TextInput* createType(std::string name, std::string type_name);

		// update the text
		void setText(std::string text);

		// frame handler
		void onFrame(float delta);

		// update cursor position
		void updateCursor(bool onlyCursor = false);

		// input handling
		bool onFocus(Event* ev, Unit* unit);
		bool onLooseFocus(Event* ev, Unit* unit);
		bool onKeyPressed(Event* ev, Unit* unit);
		bool onTextInput(Event* ev, Unit* unit);
		bool onMouseButton(Event* ev, Unit* unit);

		// events

		void setArea(bool moved, bool sized);

		void ready();

	protected:

		void updateText();

		Label* m_label_item;
		Label* m_cursor_item;
		TextWidget* m_textwidget;
		
		// the currently entered and displayed text
		std::string m_text;
		std::string m_placeholder;
		
		// position of cursor in text
		size_t m_pos;
		
		// length of text
		size_t m_length;

		// wheter the text is multiline or not
		bool multiline;

		// cursor blink timer
		bool m_cursor_shown;
		float m_cursor_timer;

		// offset for text alignement
		float m_offset_x;

		// do we have focus
		bool m_has_focus;
	};
}

#endif // !RHEA_GUI_TEXTINPUT
