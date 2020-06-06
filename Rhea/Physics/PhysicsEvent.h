#ifndef RHEA_PHYSICS_EVENT
#define RHEA_PHYSICS_EVENT

#include "../Event.h"

namespace Rhea
{
	class Terrain;

	class PhysicsEvent : public Event
	{
	public:
		PhysicsEvent(std::string e_name, float delta, Terrain* terrain) :
			m_delta(delta),
			m_terrain(terrain),
			Event(e_name)
		{}

		//getters
		float getDelta() { return m_delta; }
		Terrain* getTerrain() { return m_terrain; }

	protected:
		float m_delta;
		Terrain* m_terrain;
	};
}

#endif // !RHEA_PHYSICS_EVENT
