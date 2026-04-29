/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#include <Windows.h>

#include <Psio/Input/Input.h>
#include "Win32Input.h"
#include "../../../Input/InputEventBuilder.h"

namespace psio
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
				addEvent(BuildQuitEvent());
			}
		}
	}
}
