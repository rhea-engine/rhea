#ifndef RHEA_GUI_WIDGET_H
#define RHEA_GUI_WIDGET_H

#include "../../Unit.h"
#include "../Canvas/Item2D.h"
#include <Math/Vector2.h>
#include <SimpleParser/CssParser.h>
#include "Modifier/Modifier.h"
#include "Layout/Alignment.h"
#include "Styles/Property.h"
#include "Styles/StyleEngine.h"

using Math::Vector2;

namespace Rhea
{
	class Event;
	
	class Widget : public Item2D {
		MECLASS(Widget, Item2D)

	public:
		Widget(std::string path_name) :
			Widget(path_name, "//meworla/gui/widget")
		{}
		Widget(std::string path_name, std::string type_name) :
			Item2D(path_name, type_name),
			m_top_window(false),			
			m_handle_input(false),
			m_user_ptr(nullptr),			
			m_base_position(0.0f, 0.0f),
			m_cur_position(0.0f, 0.0f),
			m_font_size(0.0),
			m_screen_position_valid(false),
			m_style_dirty(true),
			m_is_rtl(false),
			m_essential_size_valid(false)
		{}

		virtual void setup(Application* app, std::unordered_map<std::string, std::string> attributes, std::unordered_map<std::string, Variant> values);

		// Event handling
		void ready();

		// default events
		void setOnClick(std::string target);
		virtual bool onClicked(Event* ev, Unit* unit);

		// move the current position
		void setPosition(Vector2 position);
		virtual void onSetPosition();

		// recalculate my size (get preferred content size)
		Vector2 size(Vector2 size_constraints);		
		// measure my content size
		virtual Vector2 innerSize(Vector2 size_constraints);

		// get the current size
		Vector2 getSize() {	return m_size; }
		// set the current size
		void setSize(Vector2 size) { m_size = size; }
		
		// minimal size for container layouts
		Vector2 getMinSize(Vector2 size_constraints);
		virtual void innerGetMinSize(Vector2 size_constraints, Vector2& min_size);

		Vector2 getOuterSize() { return m_outer_size; }
		Vector2 getContainingBlock() { return m_content_size; }

		// get the current position (relative to parent)
		Vector2 getBasePosition() { return m_base_position; }
		// set the position (relative to parent)
		void setBasePosition(Vector2 pos) { m_base_position = pos; }
		// get the current content position (relative to base position)
		Vector2 getContentPosition() { return m_content_position; }
		// set the content position (relative to base position)
		void setContentPosition(Vector2 pos) { m_content_position = pos; }


		// style
		void RecalculateStyle(StyleEngine* style_engine);		
		void AppendStyles(CssRuleSet& styles);
		virtual void ApplyStyles();
		// set widgets own style(s)
		void setStyles(CssRuleSet& styles);
		std::vector<CssValue>& getStyle(StylePropertyId style);
		// set the css value with text. value will be parsed
		void setStyle(StylePropertyId id, SimpleParser::Variant value);
		virtual const std::string getSelector() { return "Widget"; }
		float getFontSize() { return m_font_size; }

		// Widget handling functions

		// set the widget to be at toplevel (not a child of another widget)
		void setToplevel(bool top) { m_top_window = top; }
		// show the widget
		void show();
		// show the widget but dont send event
		void makeVisble();
		// hide the widget
		void hide();
		// hide the widget but dont send event
		void makeInvisble();		

		// find lowest widget at position
		Widget* capture(int x, int y);

		// find input target
		Widget* getInputTarget();
		void setHandleInput(bool handle) { m_handle_input = handle; }

		// event sending (for gui binding)
		void sendBoundEvent(std::string binding, Event* ev);

		// user ptr
		void setUserPtr(void* ptr) { m_user_ptr = ptr; }
		void* getUserPtr() { return m_user_ptr; }

	protected:

		// get the screen position
		Vector2 getScreenPosition();

		// wheter this is a top window
		bool m_top_window;
		
		// does widget handle input events
		bool m_handle_input;

		// defined coordinates
		
		// position of box with border (w/o margin) relative to parent
		Vector2 m_base_position;
		// size of the box with border (w/o margin)
		// Vector2 m_size defined in Item2D
		
		// position of content area relative to parent
		Vector2 m_content_position;
		// size of content area
		Vector2 m_content_size;

		// outer size of element including margin
		Vector2 m_outer_size;

		// cached font size
		float m_font_size;

		Vector2 m_cached_min_size;
		Vector2 m_cached_max_size;
		bool m_essential_size_valid;

		std::vector<std::unique_ptr<Modifier>> m_modifiers;

		// current coordinates

		// current screen position in pixel
		Vector2 m_cur_position;

		// screen space
		bool m_screen_position_valid;

		// layout
		std::unique_ptr<Alignment> m_alignment;

		// a user ptr
		void* m_user_ptr;

		// my styles
		CssRuleSet m_properties;
		// Holds the computed style properties for this widget.
		CssRuleSet m_computed_properties;
		// store if style is changed
		bool m_style_dirty;

		// rtl layout
		bool m_is_rtl;

		// event handler cache
		std::string m_onClick;
	};
}

#endif // !RHEA_GUI_WINDOW_H
