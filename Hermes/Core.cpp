#include <list>

using std::list;

#include "Core.h"
#include "Hermes/Server.h"

namespace Hermes
{
	int Core::run()
	{
		// iterate over commands
		list <Task*> ::iterator task_it = m_task_list.begin();
		list <Task*> ::iterator task_end = m_task_list.end();
		while (task_it != task_end)
		{
			Task* task = *task_it;
			task->run();
			++task_it;
		}
		if (m_task_list.empty())
			return 0;
		else
			return 1;
	}

	int Core::loop()
	{
		do
		{
			if (!run())
				break;
			
			// give time to the system
			Sleep(1);

		} while (true);
		
		return 0;
	}

	void Core::addTask(Task* task)
	{
		m_task_list.push_back(task);
	}

	void Core::closeTask(Task* task)
	{
		m_task_list.remove(task);
	}	
}