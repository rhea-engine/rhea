#include "InputMap.h"
#include "Psio/Keyboard.h"

namespace Rhea
{
	void InputMap::setAction(InputEventSP evt, std::string action)
	{
		ievt_actions act;

		auto it = m_evt_map.find(evt->hash());
		if (it == m_evt_map.end()) {
			act.evt = evt;
			act.actions.push_back(action);
			m_evt_map[evt->hash()] = act;
		}
		else {
			it->second.actions.push_back(action);
		}
	}
		
	ievt_actions InputMap::getActions(InputEventSP evt)
	{
		int hash = evt->hash();
		return m_evt_map[hash];
	}
	
	struct actdef {
		uint32_t code;
		const char* action;
	};

	actdef default_key_map[] = {
		{ Psio::CODE_LEFT, "move_left" },
		{ Psio::CODE_RIGHT, "move_right" },
		{ Psio::CODE_UP, "move_up" },
		{ Psio::CODE_DOWN, "move_down" },
		{ Psio::CODE_Q, "turn_left" },
		{ Psio::CODE_E, "turn_right" },
		{ Psio::CODE_A, "move_left" },
		{ Psio::CODE_D, "move_right" },
		{ Psio::CODE_W, "move_up" },
		{ Psio::CODE_S, "move_down" },
		{ Psio::CODE_ESC, "cancel" },
	};

	void InputMap::loadDefaultMap()
	{
		// safety check
		if (!m_evt_map.empty())
			return;

		InputEventSP evt = std::make_shared<Psio::KeyInputEvent>(Psio::InputEventId::KEY_PRESSED);
		for (int i = 0; i < sizeof(default_key_map) / sizeof(actdef); i++) {
			std::dynamic_pointer_cast<Psio::KeyInputEvent>(evt)->set(default_key_map[i].code, 0, 0);
			setAction(evt, default_key_map[i].action);
		}
	}
}