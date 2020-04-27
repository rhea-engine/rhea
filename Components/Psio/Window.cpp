#include "Window.h"
#ifdef WIN32
#  include "Win32/Win32Window.h"
#endif
#include "Input.h"

namespace Psio
{
	Window* Window::createWindow()
	{
#ifdef WIN32
		Window* win = (Window *)new Win32Window();
#endif
		return win;
	}

	void Window::setInput(Input* input)
	{
		m_input = input;
	}

	void Window::addInputEvent(std::shared_ptr<InputEvent> evt)
	{
		m_input->addEvent(evt);
	}

	void Window::onKeyDown(unsigned int key)
	{
		
	}
}