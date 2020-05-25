#ifndef RHEA_PHYSICS_BODY_H
#define RHEA_PHYSICS_BODY_H

#include <Poseidon/Position.h>

#include "Element.h"

namespace Rhea
{
	class Item;

	class PhysicsBody : public Element
	{
	public:
		PhysicsBody(Item* item);
		virtual ~PhysicsBody();

		// move by vector
		void move(Poseidon::Vector3 move);

		// turn by radian angle
		void turn(float turn);

		// stop the current movement
		virtual void stop() {};

	protected:		
		Poseidon::Vector3 m_movement;
		float m_turn;
		Item* m_item;
	};
}

#endif // !RHEA_PHYSICS_BODY_H