#ifndef RHEA_GUI_CONTROLLER_H
#define RHEA_GUI_CONTROLLER_H

#include "Widget.h"

namespace Rhea
{

	class GUIController : public Widget
	{
		MECLASS(GUIController, Widget)
	public:
		GUIController(std::string name) :
			GUIController(name, "//meworla/mwgui/controller")
		{}
		GUIController(std::string name, std::string type_name) :	
			m_widget(nullptr),
			m_target(nullptr),
			m_captured(nullptr),
			m_mx(0),
			m_my(0),
			m_viewport_width(0),
			m_viewport_height(0),
			Widget(name, type_name)
		{}

		// setup from strings
		void setup(Application* app, std::unordered_map<std::string, std::string> attributes, std::unordered_map<std::string, Variant> values);
		std::vector<Export> getReferences();

		// ready event handler
		void ready();

		// show event handler
		bool onShow(Event* ev, Unit* unit);

		// hide event handler
		bool onHide(Event* ev, Unit* unit);

		// input event handler
		bool onInput(Event* ev, Unit* unit);

		// show widget event handler
		bool onShowWidget(Event* ev, Unit* unit);

		// hide widget event handler
		bool onHideWidget(Event* ev, Unit* unit);

		// redraw requested
		bool onGuiUpdate(Event* ev, Unit* unit);

		// gui fabric function
		static void Register(Application* app);

	protected:
		Widget* m_widget;

		// the currently active widget that receives user input
		Widget* m_target;
		// the current hover widget
		Widget* m_captured;

		// last mouse position
		int m_mx;
		int m_my;

		// viewport size
		unsigned int m_viewport_width;
		unsigned int m_viewport_height;
	};
}

#endif // !RHEA_GUI_CONTROLLER_H
