#ifndef PSIO_INPUT_H
#define PSIO_INPUT_H

#include <queue>

#include "InputEvent.h"

namespace Psio
{
	class Input
	{
	public:
		std::shared_ptr<InputEvent> poll();
		void addEvent(std::shared_ptr<InputEvent> evt);
	protected:
		std::queue<std::shared_ptr<InputEvent>> m_queue;
	};
}

#endif // !PSIO_INPUT_H
