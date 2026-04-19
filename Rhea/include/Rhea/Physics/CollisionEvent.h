#ifndef RHEA_PHYSICS_COLLISION_EVENT
#define RHEA_PHYSICS_COLLISION_EVENT

#include "../Event.h"
#include <Math/Vector3.h>

namespace rhea
{
	class Item;

	class CollisionEvent : public Event
	{
	public:
		CollisionEvent(Item* self, Item* other, const math::Vector3& point, const math::Vector3& normal) :
			Event("collision", self),
			m_other(other),
			m_collision_point(point),
			m_collision_normal(normal)
		{}

		Item* getOther() const { return m_other; }
		const math::Vector3& getCollisionPoint() const { return m_collision_point; }
		const math::Vector3& getCollisionNormal() const { return m_collision_normal; }

	protected:
		Item* m_other;
		math::Vector3 m_collision_point;
		math::Vector3 m_collision_normal;
	};
}

#endif // !RHEA_PHYSICS_COLLISION_EVENT