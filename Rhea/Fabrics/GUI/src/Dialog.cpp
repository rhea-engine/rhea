#include <Rhea/GUI/Dialog.h>
#include <Rhea/Application.h>
#include <Rhea/UnitTree.h>
#include <Psio/Log.h>
#include <Rhea/EventInput.h>

#include <Rhea/GUI/Modifier/FillSize.h>

namespace rhea
{
	void Dialog::setup(Application* app, std::unordered_map<std::string, std::string> attributes, std::unordered_map<std::string, Variant> values)
	{
		Widget::setup(app, attributes, values);

		std::unordered_map<std::string, std::string> attr;
		std::unordered_map<std::string, Variant> val;
	}

	void Dialog::ready()
	{
		Widget::ready();

		// watch for esc input
		registerEventHandler("key_pressed", this, [this](Event* ev, Unit* u)->bool {
			EventInput* input_ev = static_cast<EventInput*>(ev);
			if (input_ev && input_ev->isAction("cancel")) {
				cancel();
				ev->stopPropagation();
				return true;
			}
			return false;
			});

		registerEventHandler("close", this, [this](Event* ev, Unit* u)->bool {
			hide();
			return false;
			});

		// semantic events

		registerEventHandler("submit", this, [this](Event* ev, Unit* u)->bool {
			submit();
			ev->stopPropagation();
			return true;
			});
		registerEventHandler("cancel", this, [this](Event* ev, Unit* u)->bool {
			cancel();
			ev->stopPropagation();
			return true;
			});

		// dialogs are not visible by default and must be "opened"
		makeInvisble();
	}

	void Dialog::cancel()
	{
		if (m_finished)
			return;
		m_finished = true;

		FinishedEvent ev(this, FinishedEvent::FinishType::Cancel);
		if (m_values.find("onFinished") != m_values.end()) {
			sendBoundEvent(m_values["onFinished"].toString(), &ev);
		}
		else
			sendEvent("finished", &ev);
		closeDialog();
	}

	void Dialog::submit()
	{
		if (m_finished)
			return;
		m_finished = true;

		FinishedEvent ev(this, FinishedEvent::FinishType::Submit);
		if (m_values.find("onFinished") != m_values.end()) {
			sendBoundEvent(m_values["onFinished"].toString(), &ev);
		}
		else
			sendEvent("finished", &ev);
		closeDialog();		
	}

	void Dialog::show()
	{
		Widget::show();
		m_finished = false;
	}

	// a dialog centers itself in the parents width 
	void Dialog::onSetPosition()
	{
		Widget* p = doCast<Widget>(m_parent);
		if (p) {
			Vector2 parent_size = p->getSize();

			// m_outer_size contains the size including margin
			// Calculate padding to center the dialog
			Vector2 padding = Vector2((parent_size.x - m_outer_size.x) / 2, (parent_size.y - m_outer_size.y) / 2);
			
			// Override the position calculation:
			// Instead of using anchor + base_position, we use anchor + centering offset
			// This means we ignore any authored x/y offset (m_base_position)
			m_position = m_anchor_position + padding;
		}

		// set children position
		Widget::onSetPosition();
	}
}