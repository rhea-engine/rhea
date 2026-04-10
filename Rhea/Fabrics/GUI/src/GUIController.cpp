#include <Rhea/GUI/GUIController.h>
#include <Rhea/Application.h>
#include <Rhea/UnitTree.h>
#include <Rhea/Core/Scene.h>

#include <Rhea/GUI/GuiService.h>

#include <Rhea/GUI/Widget.h>
#include <Rhea/GUI/Label.h>
#include <Rhea/Core/SceneLoader.h>
#include <Rhea/EventInput.h>
#include <Rhea/GUI/Button.h>
#include <Rhea/GUI/Dialog.h>
#include <Rhea/GUI/List.h>
#include <Rhea/GUI/ListItem.h>
#include <Rhea/GUI/Styles/StyleSheet.h>
#include <Rhea/GUI/CssLoader.h>
#include <Rhea/Graphics/IGraphicsService.h>
#include <Psio/Log.h>
#include <Rhea/GUI/TextInput.h>
#include <Rhea/GUI/Box.h>
#include <Rhea/GUI/Modifier/FillSize.h>
#include <Rhea/GUI/Column.h>
#include <Rhea/GUI/Row.h>
#include <Rhea/Graphics/SizeEvent.h>
#include <Psio/Input/Keyboard.h>

namespace rhea
{
	void GUIController::setup(Application* app, std::unordered_map<std::string, std::string> attributes, std::unordered_map<std::string, Variant> values)
	{
		m_app = app;

		Widget::setup(app, attributes, values);

		m_values["fillsize"] = true;
		m_modifiers.push_back(std::make_unique<FillSizeModifier>());

		// make sure the GUI is not displayed in the world render context by default
		m_values["rendercontext"] = getValue("rendercontext", "HUD");
	}

	std::vector<Unit::Export> GUIController::getReferences()
	{
		std::vector<Export> refs = Widget::getReferences();
		if (m_values.find("stylesheet") != m_values.end()) {
			refs.push_back({ "stylesheet", "text/css", m_values["stylesheet"] });
		}
		return refs;
	}

	void GUIController::ready()
	{
		// prepare all children
		Widget::ready();
		
		// set current viewport size
	/*	IGraphicsService* gsrv = (IGraphicsService*)m_app->getService("graphics");
		m_viewport_width = gsrv->getViewportWidth();
		m_viewport_height = gsrv->getViewportHeight();
		gsrv->registerEventHandler("resize", this, [&, gsrv](Event* ev, Unit* unit) -> bool { 
			m_viewport_width = gsrv->getViewportWidth();
			m_viewport_height = gsrv->getViewportHeight();
			return onGuiUpdate(ev, unit); 
		});
		*/
		m_style_engine = std::make_unique<StyleEngine>();
		if (m_values.find("stylesheet") != m_values.end()) {
			StyleSheet* css = (StyleSheet*)m_app->getUnitTree()->find(m_values["stylesheet"].toString(), m_attributes["basepath"], false);
			m_style_engine->AppendStylesheet(css);
		}
		else {
			LOG(LOG_ERROR) << "No stylesheet defined for GUI Controller";
		}

		// apply Style
		UpdateStyle();

		// all children are ready - set toplevel mark
		for (auto it = m_children.begin(); it != m_children.end(); ++it)
		{
			Widget* w = doCast<Widget>(it->second);
			if (w) {
				w->setToplevel(true);
			}
		}
	
		registerEventHandler("viewport_size", this, std::bind(&GUIController::onViewportSize, this, std::placeholders::_1, std::placeholders::_2));

		registerEventHandler("scene_show", this, std::bind(&GUIController::onShow, this, std::placeholders::_1, std::placeholders::_2));
		registerEventHandler("scene_hide", this, std::bind(&GUIController::onHide, this, std::placeholders::_1, std::placeholders::_2));

		// watch for updates
		registerEventHandler("scene_enter", this, std::bind(&GUIController::onSceneEnter, this, std::placeholders::_1, std::placeholders::_2));
		registerEventHandler("scene_leave", this, [&](Event* ev, Unit* u)->bool {
			rhea::Unit* target = ev->getTarget();
			// if the leaving unit is currently captured clear the reference
			target->traverse([&](std::string name, rhea::Component* c)->bool {
				if (c == m_target)
					m_target = nullptr;
				if (c == m_captured)
					m_captured = nullptr;
				if (c == m_pressed)
					m_pressed = nullptr;
				validateFocus();
				return true;
				});
			return GUIController::onGuiUpdate(ev, u);
			});
		registerEventHandler("gui_redraw", this, std::bind(&GUIController::onGuiUpdate, this, std::placeholders::_1, std::placeholders::_2));
		registerEventHandler("need_resize", this, [&](Event* ev, Unit* u)->bool {
			m_need_resize = true; return false; 
			});

		// handle show/hide of children
		registerEventHandler("show", this, std::bind(&GUIController::onShowWidget, this, std::placeholders::_1, std::placeholders::_2));
		registerEventHandler("hide", this, std::bind(&GUIController::onHideWidget, this, std::placeholders::_1, std::placeholders::_2));
	}

	bool GUIController::onViewportSize(Event* ev, Unit* unit)
	{
		if (ev) {
			SizeEvent* e = static_cast<SizeEvent*>(ev);
			int width = e->width;
			int height = e->height;
			if (width != m_viewport_width || height != m_viewport_height) {
				m_viewport_width = width;
				m_viewport_height = height;
				// send event to all children to update their size
				sendEventChildren("viewport_size", ev);
				return onGuiUpdate(ev, unit);
			}
		}
		else {
			SizeEvent e(this, m_viewport_width, m_viewport_height);
			sendEventChildren("viewport_size", &e);
			return onGuiUpdate(&e, unit);
		}
		return false;
	}


	bool GUIController::onShow(Event* ev, Unit* unit)
	{
		m_app->getUnitTree()->registerEventHandler("gui_input", this, std::bind(&GUIController::onInput, this, std::placeholders::_1, std::placeholders::_2));
		static_cast<GuiService*>(m_app->getService("gui"))->registerController(this);
		// resize
		onGuiUpdate(ev, unit);

		return false;
	}

	bool GUIController::onHide(Event* ev, Unit* unit)
	{
		m_app->getUnitTree()->unregisterEventHandler("gui_input", this);
		static_cast<GuiService*>(m_app->getService("gui"))->unregisterController(this);
		m_captured = nullptr;
		m_target = nullptr;

		return false;
	}

	bool GUIController::onInput(Event* ev, Unit* unit)
	{
		EventInput* e = static_cast<EventInput*>(ev);

		if (m_target) {

		}

		psio::InputEventId id = e->getEvent()->getId();

		if (e->isAction("fullscreen")) {
			IGraphicsService* gsrv = doCast<IGraphicsService>(m_app->getService("graphics"));
			if (gsrv) {
				gsrv->ToggleFullscreen();
			}
			return false;
		}

		// Hovering

		switch (id) {
		case psio::InputEventId::MOUSE_BTN_PRESSED:
		case psio::InputEventId::MOUSE_BTN_RELEASED:
		case psio::InputEventId::MOUSE_MOVE:
			// always check if the mouse moved
			std::shared_ptr<psio::MouseInputEvent> me = std::static_pointer_cast<psio::MouseInputEvent>(e->getEvent());
			int mx = me->getX();
			int my = me->getY();

			if (mx != m_mx || my != m_my) {
				// mouse really moved - call mouse move

				// update mouse position
				m_mx = mx;
				m_my = my;

				refreshHoverState();
			}
		}

		// Focus handling

		// handle input events

		switch(id) {
		case psio::InputEventId::MOUSE_BTN_PRESSED:

			DLOG(LOG_DEBUG) << "Mouse: x=" << m_mx << "/y=" << m_my;
			
			// do btn down on current input target
			if (m_captured) {
				// get the first focusable parent of the captured widget
				setFocus(m_captured->getFocusTarget());								
			}
			else {
				// click outside of gui - always lose focus
				setFocus(nullptr);
			}

			m_pressed = m_captured;

			if (m_pressed) {
				e->setTarget(m_pressed);
				m_pressed->bubbleEvent("mouse_btn_down", e);
			}
			break;

		case psio::InputEventId::MOUSE_BTN_RELEASED:
			DLOG(LOG_DEBUG) << "Mouse clicked";
			
			if (m_pressed) {
				e->setTarget(m_pressed);
				m_pressed->bubbleEvent("mouse_btn_up", e); // this could trigger m_pressed=nullptr

				// if the mouse is released on the same widget as it was pressed send click event
				if (m_pressed && m_pressed == m_captured) {
					auto e_click = e->cloneUnique();
					m_pressed->bubbleEvent("mouse_click", e_click.get());
				}
			}
			break;

		case psio::InputEventId::KEY_PRESSED:
			// text input
			if (m_target) {
				m_target->bubbleEvent("key_pressed", e);
			}
			else {
				// try global keys
				onKeyPressed(e, this);
			}
			break;

		case psio::InputEventId::KEY_RELEASED:
			// text input
			if (m_target) {
				m_target->bubbleEvent("key_released", e);
			}
			break;

		case psio::InputEventId::TEXT:
			// text input
			if (m_target) {
				m_target->bubbleEvent("text_input", e);
			}
			break;
		}
		
		return false;
	}

	bool GUIController::setFocus(Widget* w)
	{
		if (w == m_target)
			return true;

		if (w && !w->isFocusable())
			return false;

		if (w && !w->isEnabled())
			return false;

		Widget* old = m_target;
		m_target = w;

		if (old) {
			Event focus_lost("focus_lost", old);
			old->sendEvent("focus_lost", &focus_lost);
			LOG(LOG_DEBUG) << "Focus lost: " << old->getPath();
		}

		if (w) {
			Event focus("focus", w);
			w->sendEvent("focus", &focus);
			LOG(LOG_DEBUG) << "Focus gained: " << w->getPath();
		}

		return true;
	}

	void GUIController::clearFocus()
	{
		setFocus(nullptr);
	}

	bool GUIController::isWidgetInActiveScope(Widget* w) const
	{
		if (!w)
			return false;

		if (!m_focus_scope_root)
			return true;

		Widget* cur = w;
		while (cur) {
			if (cur == m_focus_scope_root)
				return true;			
			cur = doCast<Widget>(cur->getParent());
		}
		return false;
	}

	bool GUIController::canFocusWidget(Widget* w) const
	{
		if (!w)
			return false;

		Widget* cur = w;
		while (cur) {
			if (!cur->isEnabled() || !cur->isVisible())
				return false;
			cur = doCast<Widget>(cur->getParent());
		}

		if (!isWidgetInActiveScope(w))
			return false;

		if (!w->isFocusable())
			return false;

		return true;
	}

	void GUIController::setFocusScope(Widget* root)
	{
		if (m_focus_scope_root == root)
			return;

		m_previous_focus_before_scope = m_target;
		m_focus_scope_root = root;

		validateFocus();

		if (!m_target || !isWidgetInActiveScope(m_target)) {
			setFocus(findFirstFocusableInScope());
		}
	}

	void GUIController::clearFocusScope()
	{
		Widget* restore = m_previous_focus_before_scope;

		m_focus_scope_root = nullptr;
		m_previous_focus_before_scope = nullptr;

		if (restore && canFocusWidget(restore)) {
			setFocus(restore);
			return;
		}

		validateFocus();
	}

	void GUIController::validateFocus()
	{
		if (m_target && canFocusWidget(m_target))
			return;

		Widget* replacement = findFirstFocusableInScope();
		if (replacement) {
			setFocus(replacement);
		}
		else {
			clearFocus();
		}
	}

	void GUIController::collectFocusableWidgets(Widget* root, std::vector<Widget*>& out) const
	{
	    if (!root)
	        return;

		//if (canFocusWidget(root))
		//	LOG(LOG_DEBUG) << "can focus";

	    if (canFocusWidget(root))
	        out.push_back(root);

	    // Verwende die children() Methode, die bereits existiert
	    root->children([this, &out](std::string& name, Component* c) -> bool {
	        if (Widget* w = doCast<Widget>(c))
	            collectFocusableWidgets(w, out);
	        return true;
	    }, true);
	}

	Widget* GUIController::findFirstFocusableInScope() const
	{
		std::vector<Widget*> focusable_widgets;
		const Widget* root = m_focus_scope_root ? m_focus_scope_root : this;
		collectFocusableWidgets(const_cast<Widget*>(root), focusable_widgets);
		return focusable_widgets.empty() ? nullptr : focusable_widgets[0];
	}

	bool GUIController::focusNextWidget()
	{
		std::vector<Widget*> focusable_widgets;
		const Widget* root = m_focus_scope_root ? m_focus_scope_root : this;
		collectFocusableWidgets(const_cast<Widget*>(root), focusable_widgets);
		if (focusable_widgets.empty())
			return false;
		auto it = std::find(focusable_widgets.begin(), focusable_widgets.end(), m_target);
		if (it == focusable_widgets.end() || ++it == focusable_widgets.end())
			it = focusable_widgets.begin();
		setFocus(*it);
		return true;
	}

	bool GUIController::focusPreviousWidget()
	{
		std::vector<Widget*> focusable_widgets;
		const Widget* root = m_focus_scope_root ? m_focus_scope_root : this;
		collectFocusableWidgets(const_cast<Widget*>(root), focusable_widgets);
		if (focusable_widgets.empty())
			return false;
		auto it = std::find(focusable_widgets.begin(), focusable_widgets.end(), m_target);
		if (it == focusable_widgets.end() || it == focusable_widgets.begin())
			it = focusable_widgets.end();
		setFocus(*--it);
		return true;
	}

	void GUIController::refreshHoverState()
	{
		// new captured window
		Widget* captured = nullptr;
		Widget* root_capture = nullptr;

		// check all top level windows
		for (auto it = m_ordered_children.rbegin(); it != m_ordered_children.rend(); ++it)
		{
			Widget* w = doCast<Widget>(*it);
			if (w) {

				// get the captured widget (if in this tree)
				captured = w->capture(m_mx, m_my);
				if (captured) {
					root_capture = w;
					break;
				}
			}
		}

		// do nothing if the new widget is the old
		if (captured != m_captured) {

			// if we have old and new search for common ancestor
			// at least the controller is always a common ancestor
			if (captured) {

				DLOG(LOG_DEBUG) << "Mouse captured by " << captured->getPath();

				// inform all about leave and enter that are not common ancestors				
				Widget* ancestor = nullptr;
				if (m_captured) {
					ancestor = doCast<Widget>(captured->getCommonAncestor(m_captured));
				}
				if (ancestor) {
					// leave old captured widget
					Event leave("mouse_leave", captured);
					m_captured->sendEventSubtree(ancestor, "mouse_leave", &leave);

					// enter new captured widget
					Event enter("mouse_enter", captured);
					captured->sendEventSubtree(ancestor, "mouse_enter", &enter);
				}
				else {
					// no ancestor found - m_captured was probably null

					// enter new captured widget
					Event enter("mouse_enter", captured);
					captured->sendEventSubtree(root_capture, "mouse_enter", &enter);
				}
			}
			else if (m_captured) {

				// leave old widget
				Event leave("mouse_leave", m_captured);
				m_captured->sendEventSubtree(this, "mouse_leave", &leave);
			}

			// set the capture
			m_captured = captured;
		}
	}

	bool GUIController::onShowWidget(Event* ev, Unit* unit)
	{
		return onGuiUpdate(ev, unit);
	}

	bool GUIController::onHideWidget(Event* ev, Unit* unit)
	{
		Widget* hidden_widget = doCast<Widget>(ev->getTarget());
		
		// loose focus if focus is on child
		if (m_target && hidden_widget && (hidden_widget == m_target || m_target->isAncestor(hidden_widget))) {
			Event focus_lost("focus_lost", m_target);
			m_target->bubbleEvent("focus_lost", &focus_lost);
			m_target = nullptr;
		}
		if (m_captured && hidden_widget && (hidden_widget == m_captured || m_captured->isAncestor(hidden_widget))) {
			m_captured = nullptr;
		}
		if (m_pressed && hidden_widget && (hidden_widget == m_pressed || m_pressed->isAncestor(hidden_widget))) {
			m_pressed = nullptr;
		}

		return onGuiUpdate(ev, unit);
	}

	bool GUIController::onSceneEnter(Event* ev, Unit* unit)
	{
		if (m_style_engine) {
			Widget* w = doCast<Widget>(ev->getTarget());
			if (w) {
				w->RecalculateStyle(m_style_engine.get());							
			}
		}
		return onGuiUpdate(ev, unit);
	}

	bool GUIController::onGuiUpdate(Event* ev, Unit* unit)
	{
		// sizing with constraint of viewport size
		Vector2 vp_size((float)m_viewport_width, (float)m_viewport_height);

		if (m_viewport_width <= 0 && m_viewport_height <= 0)
			return false;

		size(vp_size);

		setPosition(Vector2(m_base_position));

		return false;
	}

	void GUIController::UpdateStyle()
	{
		if (m_style_engine)
			RecalculateStyle(m_style_engine.get());
	}

	bool GUIController::onKeyPressed(Event* ev, Unit* unit)
	{
		EventInput* inputEv = dynamic_cast<EventInput*>(ev);
		if (!inputEv)
			return false;
		psio::KeyInputEvent* e = dynamic_cast<psio::KeyInputEvent*>(inputEv->getEvent().get());
		if (!e)
			return false;

		if (e->getKeyCode() == psio::Key::CODE_TAB ) {
			if (e->isShift()) {
				if (focusPreviousWidget())
					ev->stopPropagation();
			}
			else {
				if (focusNextWidget())
					ev->stopPropagation();
			}
			return true;
		}

		return false;
	}

	void GUIController::onFrame()
	{
		// 1. style pass
		UpdateStyle();

		// 2. layout pass only if the root is dirty
		if (m_layout_dirty) {
			Vector2 vp_size((float)m_viewport_width, (float)m_viewport_height);

			if (m_viewport_width > 0 || m_viewport_height > 0) {
				size(vp_size);
				setPosition(Vector2(m_base_position));
			}

			clearLayoutDirtyRecursive();
						
			refreshHoverState();
		}

		// 3. render sync pass
		syncRenderTree();
	}
}