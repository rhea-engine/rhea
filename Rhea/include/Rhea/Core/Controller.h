#ifndef RHEA_CONTROLLER_H
#define RHEA_CONTROLLER_H

#include <Rhea/Core/Item.h>
#include <Rhea/Core/ActiveUnit.h>
#include <Psio/Input/InputEvent.h>
#include <memory>

namespace rhea
{
	// class to controll an object with user input
	class Controller : public ActiveUnit
	{
		MECLASS_VIRTUAL(Controller, ActiveUnit)
	public:
		Controller(std::string path_name, std::string type_name) :
			m_target(NULL),
			ActiveUnit(path_name, type_name)
		{}

		// handle input event (after Input Map was applied)
		virtual bool onInput(Event* ev, Unit* unit) = 0;

		// set the controlled unit
		virtual void connect(Item* target) { m_target = target; }

		void onFrame(float delta) override {}

		// get the target
		Item* getTarget() { return m_target; }

	protected:
		Item* m_target;
	};
}
#endif // !RHEA_CONTROLLER_H
