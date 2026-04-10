#ifndef RHEA_INPUTEVENT_H
#define RHEA_INPUTEVENT_H

#include "Event.h"
#include <Psio/Input/InputEvent.h>

#include <vector>
#include <memory>

namespace rhea
{
	class EventInput : public Event
	{
	public:
		EventInput(std::string e_name, std::shared_ptr<psio::InputEvent> evt, std::vector<std::string> &actions) :
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

		bool isAction(std::string action);
		bool isAction(EventInput* ev);
		std::shared_ptr<psio::InputEvent> getEvent() { return m_evt; }

	protected:
		std::shared_ptr<psio::InputEvent> m_evt;
		std::vector<std::string> m_actions;
	};
}

#endif // !RHEA_INPUTEVENT_H
