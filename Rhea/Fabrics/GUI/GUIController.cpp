#include "GUIController.h"
#include "Application.h"
#include "UnitTree.h"
#include "Scene.h"

#include "GuiService.h"

#include "GUIController.h"
#include "Widget.h"
#include "Label.h"
#include <Rhea/SceneLoader.h>
#include <EventInput.h>

namespace Rhea
{
	void GUIController::setup(Application* app, std::unordered_map<std::string, std::string> attributes, std::unordered_map<std::string, Variant> values)
	{
		Unit::setup(app, attributes, values);
		m_status = Status::SETUP;
	}
	
	void GUIController::ready()
	{
		if (m_status >= Status::READY)
			return;
		m_status = Status::READY;

		// prepare all children
		Unit::ready();

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
		m_app->getUnitTree()->registerEventHandler("gui_input", std::bind(&GUIController::onInput, this, std::placeholders::_1, std::placeholders::_2));
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

				// new aptured window
				Widget* captured = nullptr;
				Widget* root_capture = nullptr;

				// check all top level windows
				for (auto it = m_children.begin(); it != m_children.end(); ++it)
				{
					Widget* w = (Widget*)it->second;
					if (w->getValue("isWidget", false)) {

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
							Event leave("mouse_leave");
							m_captured->sendEventSubtree(ancestor, "mouse_leave", &leave);

							// enter new captured widget
							Event enter("mouse_enter");
							captured->sendEventSubtree(ancestor, "mouse_enter", &enter);
						}
						else {
							// no ancestor found - m_captured was probably null

							// enter new captured widget
							Event enter("mouse_enter");
							captured->sendEventSubtree(root_capture, "mouse_enter", &enter);
						}						
					}
					else if (m_captured) {

						// leave old widget
						Event leave("mouse_leave");
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

		if (id == Psio::InputEventId::MOUSE_BTN_PRESSED) {
			// do btn down on current input target
			if (m_captured) {
				Widget* new_target = m_captured->getInputTarget();
				if (new_target != m_target) {
					if (m_target) {
						Event focus_lost("focus_lost");
						m_target->sendEvent("focus_lost", &focus_lost);
					}
					Event focus("focus");
					new_target->sendEvent("focus", &focus);
					m_target = new_target;
				}
				if (m_target) {
					Event click("mouse_click_pressed");
					m_target->sendEvent("mouse_btn_down", &click);
				}
			}
			else {
				// click outsode of gui - always loose focus
				if (m_target) {
					Event focus_lost("focus_lost");
					m_target->sendEvent("focus_lost", &focus_lost);
					m_target = nullptr;
				}
			}
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
		SimpleParser::Type::registerType<GUIController>("//meworla/gui");

		// register widget types
		SimpleParser::Type::registerType<Widget>("//meworla/gui/widget");
		SimpleParser::Type::registerType<Label>("//meworla/gui/label");

		SimpleParser::Type::registerType<GuiService>("//meworla/gui/service"); 
		
		GuiService* service = (GuiService*)SimpleParser::Type::create("GuiService", "//meworla/gui/service");
		service->setup(app);
		app->registerService("gui", service);
	}
}