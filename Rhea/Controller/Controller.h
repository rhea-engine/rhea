#ifndef RHEA_CONTROLLER_H
#define RHEA_CONTROLLER_H

#include "../Item.h"
#include <Psio/InputEvent.h>
#include <memory>

namespace Rhea
{
	// class to controll an object with user input
	class Controller : public Unit
	{
	public:
		Controller(std::string path_name, std::string type_name) :
			m_target(NULL),
			Unit(path_name, type_name)
		{}

		// handle input (after Input Map was applied)
		virtual void onInput(std::string action, std::shared_ptr<Psio::InputEvent>& evt) = 0;

		// set the controlled unit
		virtual void connect(Item* target) { m_target = target; }

		// get the target
		Item* getTarget() { return m_target; }

	protected:
		Item* m_target;
	};
}
#endif // !RHEA_CONTROLLER_H
