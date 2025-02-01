#include "GUIController.h"
#include "GUIController.h"
#include "Application.h"
#include "UnitTree.h"
#include "Fabrics/Core/Scene.h"

#include "GuiService.h"

#include "Widget.h"
#include "Label.h"
#include <Rhea/Fabrics/Core/SceneLoader.h>
#include <EventInput.h>
#include "Button.h"
#include "Dialog.h"
#include "List.h"
#include "ListItem.h"
#include "Styles/StyleSheet.h"
#include "CssLoader.h"
#include <Graphics/GraphicsService.h>
#include <Psio/Log.h>
#include "TextInput.h"
#include "Box.h"
#include "Modifier/FillSize.h"
#include "Column.h"
#include "Row.h"

namespace Rhea
{
	void GUIController::setup(Application* app, std::unordered_map<std::string, std::string> attributes, std::unordered_map<std::string, Variant> values)
	{
		m_app = app;

		Widget::setup(app, attributes, values);

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
		GraphicsService* gsrv = (GraphicsService*)m_app->getService("graphics");
		m_viewport_width = gsrv->getViewportWidth();
		m_viewport_height = gsrv->getViewportHeight();

		m_style_engine = std::make_unique<StyleEngine>();
		if (m_values["stylesheet"] != Variant()) {
			StyleSheet* css = (StyleSheet*)m_app->getUnitTree()->find(m_values["stylesheet"], m_attributes["basepath"], false);
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
	
		// initial resize is done by "show" handler		

		registerEventHandler("scene_show", this, std::bind(&GUIController::onShow, this, std::placeholders::_1, std::placeholders::_2));
		registerEventHandler("scene_hide", this, std::bind(&GUIController::onHide, this, std::placeholders::_1, std::placeholders::_2));

		// watch for updates
		registerEventHandler("scene_enter", this, std::bind(&GUIController::onGuiUpdate, this, std::placeholders::_1, std::placeholders::_2));
		registerEventHandler("scene_leave", this, std::bind(&GUIController::onGuiUpdate, this, std::placeholders::_1, std::placeholders::_2));
		registerEventHandler("gui_redraw", this, std::bind(&GUIController::onGuiUpdate, this, std::placeholders::_1, std::placeholders::_2));

		// handle show/hide of children
		registerEventHandler("show", this, std::bind(&GUIController::onShowWidget, this, std::placeholders::_1, std::placeholders::_2));
		registerEventHandler("hide", this, std::bind(&GUIController::onHideWidget, this, std::placeholders::_1, std::placeholders::_2));
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

		Psio::InputEventId id = e->getEvent()->getId();

		switch (id) {
		case Psio::InputEventId::MOUSE_BTN_PRESSED:
		case Psio::InputEventId::MOUSE_BTN_RELEASED:
		case Psio::InputEventId::MOUSE_MOVE:
			// always check if the mouse moved
			std::shared_ptr<Psio::MouseInputEvent> me = std::static_pointer_cast<Psio::MouseInputEvent>(e->getEvent());
			int mx = me->getX();
			int my = me->getY();

			if (mx != m_mx || my != m_my) {
				// mouse really moved - call mouse move

				// new captured window
				Widget* captured = nullptr;
				Widget* root_capture = nullptr;

				// check all top level windows
				for (auto it = m_children.begin(); it != m_children.end(); ++it)
				{
					Widget* w = doCast<Widget>(it->second);
					if (w) {

						if (me->getX() > 1330) {
							captured = nullptr;
						}

						// get the captured widget (if in this tree)
						captured = w->capture(me->getX(), me->getY());
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

						// inform all about leave and enter that are not common ancestors
						Widget* ancestor = doCast<Widget>(captured->getCommonAncestor(m_captured));
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

				// store current mouse position
				m_mx = mx;
				m_my = my;
			}
		}

		switch(id) {
		case Psio::InputEventId::MOUSE_BTN_PRESSED:

			DLOG(LOG_DEBUG) << "Mouse: x=" << m_mx << "/y=" << m_my;
			
			// do btn down on current input target
			if (m_captured) {
				Widget* new_target = m_captured->getInputTarget();
				if (new_target != m_target) {
					if (m_target) {
						Event focus_lost("focus_lost", m_target);
						m_target->bubbleEvent("focus_lost", &focus_lost);
					}
					if (new_target) {
						Event focus("focus", new_target);
						new_target->bubbleEvent("focus", &focus);
					}
					m_target = new_target;
				}
				if (m_target) {
					e->setTarget(m_target);
					m_target->bubbleEvent("mouse_btn_down", e);
				}
			}
			else {
				// click outsode of gui - always loose focus
				if (m_target) {
					Event focus_lost("focus_lost", m_target);
					m_target->bubbleEvent("focus_lost", &focus_lost);
					m_target = nullptr;
				}
			}
			break;

		case Psio::InputEventId::MOUSE_BTN_RELEASED:
			DLOG(LOG_DEBUG) << "Mouse clicked";

			// call currently targeted widget
			// if none - ignore
			if (m_target) {
				e->setTarget(m_target);
				m_target->bubbleEvent("mouse_btn_up", e);
			}

		case Psio::InputEventId::KEY_PRESSED:
			// text input
			if (m_target) {
				m_target->bubbleEvent("key_pressed", e);
			}
			break;

		case Psio::InputEventId::TEXT:
			// text input
			if (m_target) {
				m_target->bubbleEvent("text_input", e);
			}
			break;

		case Psio::InputEventId::WIN_SIZE:
			// resize window
/*
			std::shared_ptr<Psio::WinSizeEvent> me = std::static_pointer_cast<Psio::WinSizeEvent>(e->getEvent());
			m_viewport_width = me->width();
			m_viewport_height = me->height();
			onGuiUpdate(ev, unit);
			*/
			break;

		}
		
		return false;
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

		return onGuiUpdate(ev, unit);
	}

	bool GUIController::onGuiUpdate(Event* ev, Unit* unit)
	{
		// sizing with constraint of viewport size
		//Vector2 vp_size((float)m_viewport_width, (float)m_viewport_height);
		GraphicsService* gsrv = (GraphicsService*)m_app->getService("graphics");
		Vector2 vp_size((float)gsrv->getViewportWidth(), (float)gsrv->getViewportHeight());
		size(vp_size);

		setPosition(Vector2(m_base_position));
		return false;
	}

	void GUIController::UpdateStyle()
	{
		RecalculateStyle(m_style_engine.get());
	}

	void GUIController::Register(Application* app)
	{
		// register widget types
		SimpleParser::Type::registerType<Widget>("//meworla/gui/widget");

		// layout types
		SimpleParser::Type::registerType<BoxWidget>("//meworla/gui/box");
		SimpleParser::Type::registerType<ColumnWidget>("//meworla/gui/column");
		SimpleParser::Type::registerType<RowWidget>("//meworla/gui/row");
	
		// basic widgets
		SimpleParser::Type::registerType<Label>("//meworla/gui/label");
		SimpleParser::Type::registerType<TextInput>("//meworla/gui/textinput");
		SimpleParser::Type::registerType<Button>("//meworla/gui/button");
		SimpleParser::Type::registerType<Dialog>("//meworla/gui/dialog");
		SimpleParser::Type::registerType<ListWidget>("//meworla/gui/list");
		SimpleParser::Type::registerType<ListWidgetItem>("//meworla/gui/listitem");

		// general gui types
		SimpleParser::Type::registerType<GUIController>("//meworla/gui");
		
		// register helpers
		app->getResourceLoader()->registerLoader<CssLoader>("text/css");
		SimpleParser::Type::registerType<GuiService>("//meworla/gui/service"); 
		
		GuiService* service = SimpleParser::Type::create<GuiService>("GuiService");
		service->setup(app);
		app->registerService("gui", service);
	}
}