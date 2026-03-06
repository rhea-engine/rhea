#include "Rhea/InputMap.h"
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

	actdef default_keypress_map[] = {
		{ Psio::CODE_LEFT, "turn_left" },
		{ Psio::CODE_RIGHT, "turn_right" },
		{ Psio::CODE_UP, "move_forward" },
		{ Psio::CODE_DOWN, "move_back" },
		{ Psio::CODE_Q, "move_left" },
		{ Psio::CODE_E, "move_right" },
		{ Psio::CODE_A, "turn_left" },
		{ Psio::CODE_D, "turn_right" },
		{ Psio::CODE_W, "move_forward" },
		{ Psio::CODE_S, "move_back" },
		{ Psio::CODE_HOME, "move_up" },
		{ Psio::CODE_END, "move_down" },
		{ Psio::CODE_ESC, "cancel" },
		{ Psio::CODE_SPACE, "jump"},
		{ Psio::CODE_F2, "debug"},
		
	};

	actdef default_keyup_map[] = {
		{ Psio::CODE_LEFT, "turn_left_stop" },
		{ Psio::CODE_RIGHT, "turn_right_stop" },
		{ Psio::CODE_UP, "move_forward_stop" },
		{ Psio::CODE_DOWN, "move_back_stop" },
		{ Psio::CODE_Q, "move_left_stop" },
		{ Psio::CODE_E, "move_right_stop" },
		{ Psio::CODE_A, "turn_left_stop" },
		{ Psio::CODE_D, "turn_right_stop" },
		{ Psio::CODE_W, "move_forward_stop" },
		{ Psio::CODE_S, "move_back_stop" },
		{ Psio::CODE_HOME, "move_up_stop" },
		{ Psio::CODE_END, "move_down_stop" },
		{ Psio::CODE_F11, "fullscreen"}
	};


	void InputMap::loadDefaultMap()
	{
		// safety check
		if (!m_evt_map.empty())
			return;

		InputEventSP evt = std::make_shared<Psio::KeyInputEvent>(Psio::InputEventId::KEY_PRESSED);
		for (int i = 0; i < sizeof(default_keypress_map) / sizeof(actdef); i++) {
			std::dynamic_pointer_cast<Psio::KeyInputEvent>(evt)->set(default_keypress_map[i].code, 0, 0);
			setAction(evt, default_keypress_map[i].action);
		}
		evt = std::make_shared<Psio::KeyInputEvent>(Psio::InputEventId::KEY_RELEASED);
		for (int i = 0; i < sizeof(default_keyup_map) / sizeof(actdef); i++) {
			std::dynamic_pointer_cast<Psio::KeyInputEvent>(evt)->set(default_keyup_map[i].code, 0, 0);
			setAction(evt, default_keyup_map[i].action);
		}
	}
}