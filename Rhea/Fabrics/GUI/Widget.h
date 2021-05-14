#ifndef RHEA_GUI_WIDGET_H
#define RHEA_GUI_WIDGET_H

#include "../../Unit.h"
#include "Vector2.h"

namespace Rhea
{
	class Item2D;
	class Event;

	class Widget : public Unit {
	
	public:
		Widget(std::string path_name) :
			Widget(path_name, "//meworla/gui/widget")
		{}
		Widget(std::string path_name, std::string type_name) :
			Unit(path_name, type_name),
			m_top_window(false),
			m_visible(false),
			m_handle_input(false),
			m_size(0.0f, 0.0f),
			m_position(0.0f,0.0f),
			m_base_size(0.0f, 0.0f),
			m_base_position(0.0f, 0.0f),
			m_screen_position(0.0f, 0.0f),
			m_screen_position_valid(false),
			m_background_item(nullptr)
		{}

		virtual void setup(Application* app, std::unordered_map<std::string, std::string> attributes, std::unordered_map<std::string, Variant> values);

		void prepare(Unit::ReadyHandler handler);

		static Widget* createType(std::string name, std::string type);

		Item2D* getItem() { return m_background_item; }

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

		// properties

		void setToplevel(bool top) { m_top_window = top; }

		// find lowest widget at position
		Widget* capture(int x, int y);

		// find input target
		Widget* getInputTarget();

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
		Vector2 m_position;
		// current  width in pixel
		Vector2 m_size;

		// screen space
		Vector2 m_screen_position;
		bool m_screen_position_valid;

		Item2D* m_background_item;
	};
}

#endif // !RHEA_GUI_WINDOW_H
