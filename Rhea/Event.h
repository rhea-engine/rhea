#ifndef RHEA_EVENT_H
#define RHEA_EVENT_H

#include <string>
#include <functional>

namespace Rhea
{
	class Unit;

	class Event
	{
	public:
		typedef std::function<bool(Event*, Unit*)> EventHandler;

		Event(std::string name) : 
			m_name(name)
		{}

		bool isEvent(std::string ev_name) { return ev_name == m_name; }

	protected:
		std::string m_name;
	};
}

#endif // !RHEA_EVENT_H
