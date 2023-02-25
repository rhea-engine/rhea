#ifndef RHEA_INPUTEVENT_H
#define RHEA_INPUTEVENT_H

#include "Event.h"
#include <Psio/InputEvent.h>

#include <vector>

namespace Rhea
{
	class EventInput : public Event
	{
	public:
		EventInput(std::string e_name, std::shared_ptr<Psio::InputEvent> evt, std::vector<std::string> &actions) :
			Event(e_name, nullptr),
			m_evt(evt),
			m_actions(actions)
		{}

		bool isAction(std::string action);
		bool isAction(EventInput* ev);
		std::shared_ptr<Psio::InputEvent> getEvent() { return m_evt; }

	protected:
		std::shared_ptr<Psio::InputEvent> m_evt;
		std::vector<std::string> m_actions;
	};
}

#endif // !RHEA_INPUTEVENT_H
