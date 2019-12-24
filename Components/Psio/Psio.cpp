// Poseidon simple input output
#include "Psio.h"
#ifdef WIN32
#include <Windows.h>
#endif

namespace Psio
{
	int Engine::tick()
	{
#ifdef WIN32
		// Windows Message Loop (NULL means check all HWNDs belonging to this context)
		MSG  msg;
		msg.message = WM_NULL;
		if (PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
			return (msg.message == WM_QUIT);
		}
		else
			return 0;
#else
		return 1;
#endif
	}
}