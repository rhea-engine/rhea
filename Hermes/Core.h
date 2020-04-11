#ifndef CORE_H
#define CORE_H

#include <list>
#include <functional>

#include "Task.h"

namespace Hermes
{
	class Core
	{
	public:
		// run one loop iteration -  return 0 if no requests are pending
		int run();
		// main loop that runs until canceled
		int loop();
		// add a command to the command list
		void addTask(Task* task);
		// close task
		void closeTask(Task* task);

	protected:
		std::list<Task*> m_task_list;
	};
}
#endif // !CORE_H
