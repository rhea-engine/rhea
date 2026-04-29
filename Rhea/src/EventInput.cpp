/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#include "Rhea/EventInput.h"

namespace rhea
{
	bool EventInput::isAction(std::string& action) const
	{
		// for empty action list fail fast
		if (!m_actions.size())
			return false;

		for (const auto& act : m_actions) {
			if (!act.compare(action))
				return true;
		}
		return false;
	}

	bool EventInput::isAction(std::string action) const
	{
		// for empty action list fail fast
		if (!m_actions.size())
			return false;
		
		for (const auto& act : m_actions) {
			if (!act.compare(action))
				return true;
		}
		return false;
	}
	
	bool EventInput::isAction(EventInput* ev) const
	{
		return (m_actions==ev->m_actions);
	}

}