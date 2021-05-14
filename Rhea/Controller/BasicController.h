#ifndef RHEA_BASICCONTROLLER_H
#define RHEA_BASICCONTROLLER_H

#include "Controller.h"
#include <Poseidon\Math\Vector3.h>
#include <Poseidon\Math\Quaternion.h>
#include <Poseidon\Position.h>
#include <Poseidon/Core.h>

namespace Rhea
{
	class BasicController : public Controller
	{
	public:
		BasicController(std::string path_name, std::string type_name) :
			m_position(NULL),
			m_orientation(),
			Controller(path_name, type_name)
		{}

		virtual void show(Poseidon::Core* core);
		void message(std::string method_name);

		// set the position manually
		void setPosition(Poseidon::Vector3 position);
		void lookAt(Poseidon::Vector3& target);

		Poseidon::Vector3 getPosition();

		// Controller functions

		// input event handler
		bool onInput(Event* ev, Unit* unit);

		void connect(Item* target);

		// factory function
		static Unit* createType(std::string name, std::string type);

		void logMe();		

	protected:
		Poseidon::Position m_position;
		Poseidon::Quaternion m_orientation;

		Poseidon::Vector3 m_move;
		float m_turn = 0;

		// register event handler
		virtual void _registerEvent(Unit* unittree);
	};
}

#endif // !RHEA_BASICCONTROLLER_H
