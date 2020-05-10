#ifndef RHEA_SPECTATORCONTROLLER_H
#define RHEA_SPECTATORCONTROLLER_H

#include "BasicController.h"
#include "../SceneView.h"
#include <Poseidon\Math\Vector3.h>
#include <Poseidon\Math\Quaternion.h>
#include <Poseidon\Position.h>

namespace Rhea
{
	class SpectatorController : public BasicController
	{
	public:
		SpectatorController(std::string path_name, std::string type_name) :
			/*m_position(NULL),
			m_orientation(),*/
			m_view(NULL),
			BasicController(path_name, type_name)
		{}

		void show(Poseidon::Core* core);
		void message(std::string method_name);

		// set the position manually
		void setPosition(Poseidon::Vector3 position);
		void lookAt(Poseidon::Vector3& target);

		// Controller functions

		void onInput(std::string action, std::shared_ptr<Psio::InputEvent>& evt);
		void onPhysics(float delta);

		// factory function
		static Unit* createType(std::string name, std::string type);

	protected:
		//Poseidon::Position m_position;
		//Poseidon::Quaternion m_orientation;

		SceneView* m_view;

		/*Poseidon::Vector3 m_move;
		float m_turn = 0;	*/
	};
}

#endif // !RHEA_SPECTATORCONTROLLER_H
