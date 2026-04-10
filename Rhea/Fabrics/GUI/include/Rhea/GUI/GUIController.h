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
			m_target(nullptr),
			m_captured(nullptr),
			m_pressed(nullptr),
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
		
		// set the currently active widget that receives user input
		bool setFocus(Widget* w);

		// clear the currently active widget
		void clearFocus();

		// get the currently active widget that receives user input
		Widget* getFocusedWidget() const { return m_target; }

		// check if a widget is within the current focus scope
		bool isWidgetInActiveScope(Widget* w) const;

		// check if a widget can receive focus (is visible and enabled)
		bool canFocusWidget(Widget* w) const;

		// set a new focus scope with the given root widget
		void setFocusScope(Widget* root);

		// clear the current focus scope and restore the previous focus
		void clearFocusScope();

		// validate the current focus and clear it if the focused widget is no longer valid
		void validateFocus();

		// collect all focusable widgets within the current focus scope
		void collectFocusableWidgets(Widget* root, std::vector<Widget*>& out) const;

		// find the first focusable widget within the current focus scope
		Widget* findFirstFocusableInScope() const;

		// move focus to the next or previous widget in the current focus scope
		bool focusNextWidget();
		bool focusPreviousWidget();

		// update hover state based on current mouse position
		void refreshHoverState();

		// show widget event handler
		bool onShowWidget(Event* ev, Unit* unit);

		// hide widget event handler
		bool onHideWidget(Event* ev, Unit* unit);

		// widget added to gui
		bool onSceneEnter(Event* ev, Unit* unit);

		// redraw requested
		bool onGuiUpdate(Event* ev, Unit* unit);

		// update style of elements
		void UpdateStyle();

		// handle onFrame
		void onFrame();

		// gui fabric function
		static void Register(Application* app);

	protected:

		// the currently focused widget that receives user input
		Widget* m_target;

		// the root of the current focus scope, if any
		Widget* m_focus_scope_root = nullptr;

		// the previously focused widget before entering the current focus scope
		Widget* m_previous_focus_before_scope = nullptr;

		// the current hover widget
		Widget* m_captured;		

		// handle key press events for focus navigation and other global shortcuts
		bool onKeyPressed(Event* ev, Unit* u) override;

		// if we need a resize in the next frame?
		bool m_need_resize;

		// last mouse position
		int m_mx;
		int m_my;

		std::unique_ptr<StyleEngine> m_style_engine;
	};
}

#endif // !RHEA_GUI_CONTROLLER_H
