#ifndef RHEA_INPUTEVENT_H
#define RHEA_INPUTEVENT_H

#include "Event.h"
#include <Rhea/Input/InputEvent.h>

#include <vector>
#include <string>
#include <memory>

namespace rhea
{
	class EventInput : public Event
	{
	public:
		EventInput(std::string e_name, const InputEvent& evt, std::vector<std::string> &actions) :
			Event(e_name, nullptr),
			m_evt(evt),
			m_actions(actions)
		{}

		EventInput(const EventInput& other) :
			Event(other.m_name, other.m_target),
			m_evt(other.m_evt),
			m_actions(other.m_actions)
		{}

		EventInput* clone() const {
			return new EventInput(*this);
		}

		std::unique_ptr<EventInput> cloneUnique() const {
			return std::make_unique<EventInput>(*this);
		}

		bool isAction(std::string& action) const;
		bool isAction(std::string action) const;
		bool isAction(EventInput* ev) const;
		const InputEvent& getEvent() const { return m_evt; }

		bool isKeyPressed(Key key) const { return m_evt.isKeyPressed(key); }
		bool isKeyReleased(Key key) const { return m_evt.isKeyReleased(key); }
		bool isMousePressed(MouseButton btn) const { return m_evt.isMousePressed(btn); }
		bool isMouseReleased(MouseButton btn) const { return m_evt.isMouseReleased(btn); }
		bool isTextEntered() const { return m_evt.isTextEntered(); }

	protected:
		InputEvent m_evt;
		std::vector<std::string> m_actions;
	};
}

#endif // !RHEA_INPUTEVENT_H
