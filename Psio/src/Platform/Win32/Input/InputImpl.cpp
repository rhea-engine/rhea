#include <Windows.h>

#include <Psio/Input/Input.h>
#include "InputWin32.h"

namespace Psio
{
	void InputWin32::update()
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
				addEvent(evt);
			}
		}
	}
}
