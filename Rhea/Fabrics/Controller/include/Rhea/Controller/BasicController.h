#ifndef RHEA_BASICCONTROLLER_H
#define RHEA_BASICCONTROLLER_H

#include <Rhea/Core/Controller.h>
#include <Math/Vector3.h>
#include <Math/Quaternion.h>
#include <Rhea/Core/Position.h>

namespace rhea
{
	class BasicController : public Controller
	{
		MECLASS(BasicController, Controller)
	public:
		BasicController(std::string path_name, std::string type_name) :
			m_position(),
			m_orientation(),
			Controller(path_name, type_name)
		{}

		virtual void show();
		void message(std::string method_name);

		// set the position manually
		void setPosition(math::Vector3 position);
		void lookAt(math::Vector3& target);

		math::Vector3 getPosition();

		// Controller functions

		// input event handler
		bool onInput(Event* ev, Unit* unit);

		void onFrame(float delta) override {};

		void connect(Item* target);

		void logMe();

		// get the current orientation
		math::Quaternion getOrientation() {	return m_orientation; }

	protected:
		Position m_position;
		math::Quaternion m_orientation;

		math::Vector3 m_move;
		float m_turn = 0;
		math::Vector3 m_last_move;
		float m_last_turn = 0;

		// register event handler
		virtual void _registerEvent(Unit* unittree);
	};
}

#endif // !RHEA_BASICCONTROLLER_H
