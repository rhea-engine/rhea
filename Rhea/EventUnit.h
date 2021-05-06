#ifndef RHEA_EVENTUNIT_H
#define RHEA_EVENTUNIT_H

#include "Event.h"

namespace Rhea {
	class EventUnit : public Event
	{
	public:
		EventUnit(std::string name, Unit* target) :
			Event(name),
			m_target(target)
		{}
	
		Unit* getTarget() { return m_target; }

	protected:
		Unit* m_target;
	};
}

#endif //  RHEA_EVENTUNIT_H