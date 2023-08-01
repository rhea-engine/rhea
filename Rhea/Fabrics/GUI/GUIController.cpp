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
#include "StyleSheet.h"
#include "CssLoader.h"
#include <Graphics/GraphicsService.h>
#include <Psio/Log.h>
#include "TextInput.h"

namespace Rhea
{
	void GUIController::setup(Application* app, std::unordered_map<std::string, std::string> attributes, std::unordered_map<std::string, Variant> values)
	{
		m_app = app;
		size();
		values["x"] = 0;
		values["y"] = 0;
		values["w"] = m_size.x;
		values["h"] = m_size.y;

		Widget::setup(app, attributes, values);		
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

		// apply Style
		if (m_values["stylesheet"] != Variant()) {
			StyleSheet* css = (StyleSheet*)m_app->getUnitTree()->find(m_values["stylesheet"], m_attributes["basepath"], false);
			if (css) {
				traverse([&, css](std::string name, Rhea::Unit* unit)->bool {
					if (unit->getValue("isWidget")) {
						css->applyStyle((Widget*)unit);
						return true;
					}
					else if (unit == this) {
						return true;
					}
					else {
						return false;
					}
					});
			}
			else {
				LOG(LOG_ERROR) << "No stylesheet defined for GUI Controller";
			}
		}

		// all children are ready - initial resize		
		for (auto it = m_children.begin(); it != m_children.end(); ++it)
		{
			Widget* w = (Widget*)it->second;
			if (w->getValue("isWidget", false)) {
				w->setToplevel(true);
				w->onParentSized(nullptr);
				w->onParentMoved();
			}
		}
		m_app->getUnitTree()->registerEventHandler("gui_input", this, std::bind(&GUIController::onInput, this, std::placeholders::_1, std::placeholders::_2));
		static_cast<GuiService*>(m_app->getService("gui"))->registerController(this);
	}

	Vector2 GUIController::size()
	{
		GraphicsService* gsrv = (GraphicsService*)m_app->getService("graphics");
		m_size.x = gsrv->getViewportWidth();
		m_size.y = gsrv->getViewportHeight();
		return m_size;
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
					Widget* w = (Widget*)it->second;
					if (w->getValue("isWidget", false)) {

						if (me->getX() > 330) {
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
						Widget* ancestor = static_cast<Widget*>(captured->getCommonAncestor(m_captured));
						if (ancestor && ancestor->getValue("isWidget"), false) {

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

		}
		
		return false;
	}

	Unit* GUIController::createType(std::string name, std::string type)
	{
		Unit* ret = (Unit*)new GUIController(name, type);
		return ret;
	}

	void GUIController::Register(Application* app)
	{
		// general gui types
		SimpleParser::Type::registerType<GUIController>("//meworla/gui");
		SimpleParser::Type::registerType<StyleSheet>("//meworla/gui/stylesheet");
		app->getResourceLoader()->registerLoader<CssLoader>("text/css");

		// register widget types
		SimpleParser::Type::registerType<Widget>("//meworla/gui/widget");
		SimpleParser::Type::registerType<Label>("//meworla/gui/label");
		SimpleParser::Type::registerType<TextInput>("//meworla/gui/textinput");
		SimpleParser::Type::registerType<Button>("//meworla/gui/button");
		SimpleParser::Type::registerType<Dialog>("//meworla/gui/dialog");
		SimpleParser::Type::registerType<ListWidget>("//meworla/gui/list");
		SimpleParser::Type::registerType<ListWidgetItem>("//meworla/gui/listitem");

		SimpleParser::Type::registerType<GuiService>("//meworla/gui/service"); 
		
		GuiService* service = (GuiService*)SimpleParser::Type::create("GuiService", "//meworla/gui/service");
		service->setup(app);
		app->registerService("gui", service);
	}
}