/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#ifndef RHEA_KINEMATIC_CHARACTER_H
#define RHEA_KINEMATIC_CHARACTER_H

#include <Rhea/Core/Position.h>
#include <Rhea/Core/Item.h>

namespace rhea
{
	/* a character that is controlled by the player or some program
	 *
	 * the physics is handled with a kinematic object
	 *
	 * normally it has a graphical represantation.
	 * a player handles it via a controller
	 */
	class KinematicCharacter : public Item
	{
		MECLASS(KinematicCharacter, Item)
	public:
		enum class MoveState {
			GROUND,
			FALL,
			FLY,
			JUMP,
		};

		KinematicCharacter(std::string path_name, std::string type_name) :
			m_move_state(MoveState::GROUND),
			m_gravity(0.0f),
			m_upwards(0.0f),
			m_air_timer(0.0f),
			m_jump_start_y(0.0f),
			m_vertical_velocity(0.0f),
			m_phy_pre_handler(nullptr),
			Item(path_name, type_name)
		{}
		virtual ~KinematicCharacter();

		void setup(Application* app, std::unordered_map<std::string, std::string> attributes, std::unordered_map<std::string, Variant> values) override;

		// handle physics update
		bool doPhysics(Event* ev, Unit* unit);

		// movement
		void jump();

		// ready event handler
		void ready();

		void onActivate() override;
		void onDeactivate() override;

	protected:
		MoveState m_move_state;
		math::Vector3 m_forced_movement;		
		float m_gravity;
		float m_upwards;
		float m_jump_start_y;
		float m_vertical_velocity;

		float m_air_timer;

		Event::EventHandler m_phy_pre_handler;
	};
}
#endif // !RHEA_KINEMATIC_CHARACTER_H
