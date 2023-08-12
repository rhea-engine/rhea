#ifndef RHEA_GUI_WIDGET_H
#define RHEA_GUI_WIDGET_H

#include "../../Unit.h"
#include "../Canvas/Item2D.h"
#include <Math/Vector2.h>
#include <SimpleParser/CssParser.h>

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
			m_visible(true),
			m_handle_input(false),		
			m_user_ptr(nullptr),
			m_base_size(0.0f, 0.0f),
			m_base_position(0.0f, 0.0f),
			m_cur_position(0.0f, 0.0f),
			m_screen_position_valid(false),	
			m_is_rtl(false)
		{}

		virtual void setup(Application* app, std::unordered_map<std::string, std::string> attributes, std::unordered_map<std::string, Variant> values);

		void ready();

		// events

		// parent has changed its size
		virtual void onParentSized(Event* event);
		// parent has been moved - used to recalculate screen coords
		virtual void onParentMoved();

		// we changed our size
		void onSized();

		// move the current position
		void setPosition(Vector2 position);

		// we moved our position
		void onMoved();

		// notify children of resize
		void sizeChildren();
		// notify children of move
		void moveChildren();

		// recalculate my size
		virtual Vector2 size();

		// get the current size
		Vector2 getSize() {	return m_size; }

		// style
		void setStyle(SimpleParser::CssParser::PropertyMap& properties);
		std::vector<Variant>* getStyle(std::string name);

		// Widget handling functions

		// set the widget to be at toplevel (not a child of another widget)
		void setToplevel(bool top) { m_top_window = top; }
		// show the widget
		void show();
		// hide the widget
		void hide();

		bool isVisible() { return m_visible; }

		// find lowest widget at position
		Widget* capture(int x, int y);

		// find input target
		Widget* getInputTarget();

		// event sending (for gui binding)
		void sendBoundEvent(std::string binding, Event* ev);

		// user ptr
		void setUserPtr(void* ptr) { m_user_ptr = ptr; }
		void* getUserPtr() { return m_user_ptr; }

	protected:

		// get the screen position
		Vector2 getScreenPosition();

		// reapply the current position and size properties for our geometry
		virtual void setArea(bool moved, bool sized);

		// wheter this is a top window
		bool m_top_window;

		// is widget visible
		bool m_visible;

		// does widget handle input events
		bool m_handle_input;

		// defined coordinates
		
		// position of element relative to parent
		Vector2 m_base_position;		
		// initial size of element
		Vector2 m_base_size;

		// current coordinates

		// current position in pixel
		Vector2 m_cur_position;

		// screen space
		bool m_screen_position_valid;

		// a user ptr
		void* m_user_ptr;

		// my styles
		SimpleParser::CssParser::PropertyMap m_properties;

		// rtl layout
		bool m_is_rtl;
	};
}

#endif // !RHEA_GUI_WINDOW_H
