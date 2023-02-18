#ifndef RHEA_CONTROLLER_H
#define RHEA_CONTROLLER_H

#include <Rhea/Fabrics/3D/Item.h>
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

		// handle input event (after Input Map was applied)
		virtual bool onInput(Event* ev, Unit* unit) = 0;

		// set the controlled unit
		virtual void connect(Item* target) { m_target = target; }

		// get the target
		Item* getTarget() { return m_target; }

	protected:
		Item* m_target;
	};
}
#endif // !RHEA_CONTROLLER_H
