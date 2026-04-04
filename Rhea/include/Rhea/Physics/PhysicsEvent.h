#ifndef RHEA_PHYSICS_EVENT
#define RHEA_PHYSICS_EVENT

#include "../Event.h"

namespace rhea
{
	class PhysicsEvent : public Event
	{
	public:
		PhysicsEvent(std::string e_name, float delta) :
			m_delta(delta),			
			Event(e_name, (Unit*)nullptr)
		{}

		//getters
		float getDelta() { return m_delta; }		

	protected:
		float m_delta;		
	};
}

#endif // !RHEA_PHYSICS_EVENT