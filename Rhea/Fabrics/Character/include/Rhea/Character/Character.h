#ifndef RHEA_FABRICS_CHARACTER_H
#define RHEA_FABRICS_CHARACTER_H

#include <Rhea/Core/Position.h>
#include <Rhea/Core/Item.h>

namespace rhea
{	
	/* a character that is controlled by the player or some program
	 *
	 * normally it has a graphical represantation. 
	 * a player handles it via a controller
	 */
	class Character : public Item
	{
		MECLASS(Character, Item)
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

	protected:
		MoveState m_move_state;
		Math::Vector3 m_forced_movement;
		rhea::Position* m_position;
		float m_gravity;
		float m_upwards;

		Event::EventHandler m_phy_pre_handler;		

		float m_air_timer;
	};
}

#endif // !RHEA_FABRICS_CHARACTER_H
