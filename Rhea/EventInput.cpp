#include "EventInput.h"

namespace Rhea
{
	bool EventInput::isAction(std::string action)
	{
		// for empty action list fail fast
		if (!m_actions.size())
			return false;

		for (auto& act : m_actions) {
			if (act == action)
				return true;
		}
		return false;
	}	
}