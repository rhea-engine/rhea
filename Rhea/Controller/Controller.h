#ifndef RHEA_CONTROLLER_H
#define RHEA_CONTROLLER_H

#include "../Unit.h"
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

		// process physics with delta seconds (usually 1/60)
		virtual void onPhysics(float delta) = 0;

		// set the controlled unit
		virtual void connect(Unit* target) { m_target = target; }

	protected:
		Unit* m_target;
	};
}
#endif // !RHEA_CONTROLLER_H
