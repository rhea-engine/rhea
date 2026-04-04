#ifndef RHEA_GUI_CONTROLLER_H
#define RHEA_GUI_CONTROLLER_H

#include "Widget.h"
#include "Styles/StyleEngine.h"

namespace rhea
{

	class GUIController : public Widget
	{
		RECLASS(GUIController, Widget, "//meworla/mwgui/controller")
	public:
		GUIController(std::string name, std::string type_name) :	
			m_widget(nullptr),
			m_target(nullptr),
			m_captured(nullptr),
			m_mx(0),
			m_my(0),
			m_need_resize(false),
			Widget(name, type_name)
		{}

		// setup from strings
		void setup(Application* app, std::unordered_map<std::string, std::string> attributes, std::unordered_map<std::string, Variant> values);
		std::vector<Export> getReferences();

		// ready event handler
		void ready();

		// viewport size event handler
		bool onViewportSize(Event* ev, Unit* unit);

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

		// update style of elements
		void UpdateStyle();

		// handle onFrame
		void onFrame();

		// gui fabric function
		static void Register(Application* app);

	protected:
		Widget* m_widget;

		// the currently active widget that receives user input
		Widget* m_target;
		// the current hover widget
		Widget* m_captured;

		// if we need a resize in the next frame?
		bool m_need_resize;

		// last mouse position
		int m_mx;
		int m_my;

		std::unique_ptr<StyleEngine> m_style_engine;
	};
}

#endif // !RHEA_GUI_CONTROLLER_H
