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

		Event(std::string name, Unit* target) : 
			m_name(name),
			m_target(target),
			m_is_stopped(false)
		{}

		bool isEvent(std::string ev_name) { return ev_name == m_name; }

		std::string getName() { return m_name; }

		Unit* getTarget() { return m_target; }

		void stopPropagation() { m_is_stopped = true; }
		bool isPropagationStoppped() { return m_is_stopped; }

	protected:
		std::string m_name;
		bool m_is_stopped;		
		Unit* m_target;
	};
}

#endif // !RHEA_EVENT_H
