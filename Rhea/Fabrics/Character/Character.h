#ifndef RHEA_FABRICS_CHARACTER_H
#define RHEA_FABRICS_CHARACTER_H

#include <Rhea/Fabrics/Core/Position.h>

#include "Fabrics/3D/Item.h"

namespace Rhea
{	
	/* a character that is controlled by the player or some program
	 *
	 * normally it has a graphical represantation. 
	 * a player handles it via a controller
	 */
	class Character : public Item
	{
	public:
		enum class MoveState {
			GROUND,
			FALL,
			FLY,
			JUMP,
		};

		Character(std::string path_name, std::string type_name) :
			m_move_state(MoveState::GROUND),
			m_position(nullptr),
			m_gravity(0.0f),
			m_upwards(0.0f),
			m_air_timer(0.0f),
			m_phy_pre_handler(nullptr),
			Item(path_name, type_name)
		{}
		virtual ~Character();

		// handle physics update
		bool doPhysics(Event* ev, Unit* unit);

		// ready event handler
		void ready();

		// scene reads
		bool onSceneReady(Event* ev, Unit* unit);

		// factory function
		static Unit* createType(std::string name, std::string type);

	protected:
		MoveState m_move_state;
		Math::Vector3 m_forced_movement;
		Rhea::Position* m_position;
		float m_gravity;
		float m_upwards;

		Event::EventHandler m_phy_pre_handler;		

		float m_air_timer;
	};
}

#endif // !RHEA_FABRICS_CHARACTER_H
