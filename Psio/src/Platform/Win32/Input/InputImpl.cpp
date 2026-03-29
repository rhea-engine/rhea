#include <Windows.h>

#include <Psio/Input/Input.h>

namespace Psio
{
	std::shared_ptr<InputEvent> Input::poll()
	{
		// Windows Message Loop (NULL means check all HWNDs belonging to this context)
		MSG  msg;
		msg.message = WM_NULL;
		while (PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
			if (msg.message == WM_QUIT) {
				std::shared_ptr<InputEvent> evt = InputEvent::createEvent(InputEventId::WMGR_QUIT);
				return evt;
			}
		}

		if (m_queue.empty())
			return NULL;
		std::shared_ptr<InputEvent> evt = m_queue.front();
		m_queue.pop();

		return evt;
	}

	void Input::addEvent(std::shared_ptr<InputEvent> evt)
	{
		m_queue.push(evt);
	}
}
