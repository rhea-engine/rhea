#ifndef RHEA_EVENT_H
#define RHEA_EVENT_H

#include "Object.h"

namespace Rhea
{
	class Event : public Object
	{
	protected:
		char* m_name;
		char* m_classname;
	public:
		Event(char* name) : m_name(name), m_classname("Event") {}
	};
}

#endif // !RHEA_EVENT_H
