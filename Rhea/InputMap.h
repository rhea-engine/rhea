#ifndef RHEA_INPUTMAP_H
#define RHEA_INPUTMAP_H

#include <unordered_map>
#include <map>
#include <vector>
#include <memory>
#include <string>

#include <Psio/InputEvent.h>

namespace Rhea {

	typedef std::shared_ptr<Psio::InputEvent> InputEventSP;

	struct ievt_actions {
		InputEventSP evt;
		std::vector<std::string> actions;
	};

	class InputMap {
	public:
		// set an action for an event
		void setAction(InputEventSP evt, std::string action);

		// get all actions registered for this event
		ievt_actions getActions(InputEventSP evt);

		// load default event configuration
		void loadDefaultMap();

	protected:
		std::map<int, ievt_actions> m_evt_map;

	};
}
#endif // !RHEA_INPUTMAP_H
