#include "Window.h"
#ifdef WIN32
#  include "Win32Window.h"
#endif

namespace Psio
{
	Window* Window::createWindow()
	{
#ifdef WIN32
		Window* win = (Window *)new Win32Window();
#endif
		return win;
	}

	void Window::onKeyDown(unsigned int key)
	{

	}
}