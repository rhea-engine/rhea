#ifndef PSIO_INPUT_H
#define PSIO_INPUT_H

#include <queue>

#include "InputEvent.h"

namespace psio
{
	class Input
	{
	public:
		virtual ~Input() = default;

		virtual void update() = 0;

		std::shared_ptr<InputEvent> poll()
		{
			if (m_queue.empty())
				return {};

			auto event = m_queue.front();
			m_queue.pop();
			return event;
		}
		void addEvent(std::shared_ptr<InputEvent> evt) { if (evt) m_queue.push(std::move(evt)); }
	protected:
		std::queue<std::shared_ptr<InputEvent>> m_queue;
	};
}

#endif // !PSIO_INPUT_H
