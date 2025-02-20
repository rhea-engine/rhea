#include "EventInput.h"

namespace Rhea
{
	bool EventInput::isAction(std::string action)
	{
		// for empty action list fail fast
		if (!m_actions.size())
			return false;

		for (auto& act : m_actions) {
			if (!act.compare(action))
				return true;
		}
		return false;
	}
	
	bool EventInput::isAction(EventInput* ev)
	{
		return (m_actions==ev->m_actions);
	}

}