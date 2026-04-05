#include "Rhea/InputMap.h"
#include <Psio/Input/Keyboard.h>

namespace rhea
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

	actdef default_keypress_map[] = {
		{ psio::CODE_LEFT, "turn_left" },
		{ psio::CODE_RIGHT, "turn_right" },
		{ psio::CODE_UP, "move_forward" },
		{ psio::CODE_DOWN, "move_back" },
		{ psio::CODE_Q, "move_left" },
		{ psio::CODE_E, "move_right" },
		{ psio::CODE_A, "turn_left" },
		{ psio::CODE_D, "turn_right" },
		{ psio::CODE_W, "move_forward" },
		{ psio::CODE_S, "move_back" },
		{ psio::CODE_HOME, "move_up" },
		{ psio::CODE_END, "move_down" },
		{ psio::CODE_ESC, "cancel" },
		{ psio::CODE_SPACE, "jump"},
		{ psio::CODE_F2, "debug"},
		
	};

	actdef default_keyup_map[] = {
		{ psio::CODE_LEFT, "turn_left_stop" },
		{ psio::CODE_RIGHT, "turn_right_stop" },
		{ psio::CODE_UP, "move_forward_stop" },
		{ psio::CODE_DOWN, "move_back_stop" },
		{ psio::CODE_Q, "move_left_stop" },
		{ psio::CODE_E, "move_right_stop" },
		{ psio::CODE_A, "turn_left_stop" },
		{ psio::CODE_D, "turn_right_stop" },
		{ psio::CODE_W, "move_forward_stop" },
		{ psio::CODE_S, "move_back_stop" },
		{ psio::CODE_HOME, "move_up_stop" },
		{ psio::CODE_END, "move_down_stop" },
		{ psio::CODE_F11, "fullscreen"}
	};


	void InputMap::loadDefaultMap()
	{
		// safety check
		if (!m_evt_map.empty())
			return;

		InputEventSP evt = std::make_shared<psio::KeyInputEvent>(psio::InputEventId::KEY_PRESSED);
		for (int i = 0; i < sizeof(default_keypress_map) / sizeof(actdef); i++) {
			std::dynamic_pointer_cast<psio::KeyInputEvent>(evt)->set(default_keypress_map[i].code, 0, 0);
			setAction(evt, default_keypress_map[i].action);
		}
		evt = std::make_shared<psio::KeyInputEvent>(psio::InputEventId::KEY_RELEASED);
		for (int i = 0; i < sizeof(default_keyup_map) / sizeof(actdef); i++) {
			std::dynamic_pointer_cast<psio::KeyInputEvent>(evt)->set(default_keyup_map[i].code, 0, 0);
			setAction(evt, default_keyup_map[i].action);
		}
	}
}