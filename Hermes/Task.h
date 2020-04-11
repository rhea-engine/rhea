#ifndef HERMES_TASK_H
#define HERMES_TASK_H

namespace Hermes
{
	class Task
	{
	public:
		// run the task - this is called on every loop iteration
		virtual void run() = 0;
	};
}

#endif // !HERMES_TASK_H
