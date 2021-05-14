#ifndef RHEA_SPECTATORCONTROLLER_H
#define RHEA_SPECTATORCONTROLLER_H

#include "BasicController.h"
#include <Poseidon\Math\Vector3.h>
#include <Poseidon\Math\Quaternion.h>
#include <Poseidon\Position.h>

namespace Rhea
{
	class Camera;

	class SpectatorController : public BasicController
	{
	public:
		SpectatorController(std::string path_name, std::string type_name) :
			m_view(NULL),
			BasicController(path_name, type_name)
		{}

		void show(Poseidon::Core* core);
		void message(std::string method_name);

		// set the position manually
		void setPosition(Poseidon::Vector3 position);
		void lookAt(Poseidon::Vector3& target);

		// Controller functions

		// input event handler
		bool onInput(Event* ev, Unit* unit);
		
		// factory function
		static Unit* createType(std::string name, std::string type);

	protected:
		Camera* m_view;

		// register event handler
		void _registerEvent(Unit* unittree);
	};
}

#endif // !RHEA_SPECTATORCONTROLLER_H
