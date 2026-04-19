#ifndef RHEA_GUI_TEXTINPUT
#define RHEA_GUI_TEXTINPUT

#include "Widget.h"
#include <Rhea/UnitTree.h>

namespace rhea
{
	class TextWidget;
	class Label;

	class TextInput : public Widget {
		MECLASS(TextInput, Widget)
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
		{
			event_binding.target = nullptr;
		}

		virtual void setup(Application* app, std::unordered_map<std::string, std::string> attributes, std::unordered_map<std::string, Variant> values);

		// update the text
		void setText(std::string text);

		// frame handler
		void onFrame(float delta);

		// recalculate my size
		Vector2 innerSize(Vector2 size_constraints);

		// reset the textWidget
		void onSetPosition();

		// update cursor position
		void updateCursor(bool onlyCursor = false);

		// input handling
		bool onFocusGained(Event* ev, Unit* unit) override;
		bool onFocusLost(Event* ev, Unit* unit) override;
		bool onKeyPressed(Event* ev, Unit* unit) override;
		bool onTextInput(Event* ev, Unit* unit) override;
		bool onMousePressed(Event* ev, Unit* unit) override;		

		// events

		void ready();

		// styling
		const std::string getSelector() { return "Input"; }

		virtual bool isFocusable() const override { return true; }
		virtual bool acceptsTextInput() const override { return true; }

	protected:

		void updateText(bool isEvent);

		Label* m_label_item;
		Label* m_cursor_item;
		TextWidget* m_textwidget;
		
		// the currently entered and displayed text
		std::string m_text;
		std::string m_placeholder;
		
		// position of cursor in text
		size_t m_pos;
		// pixel offset of cursor
		Vector2 m_cursor_position;
		
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

		// event handling
		UnitTree::Binding event_binding;
	};
}

#endif // !RHEA_GUI_TEXTINPUT
