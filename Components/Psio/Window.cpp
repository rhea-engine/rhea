#include "Window.h"
#ifdef WIN32
#  include "Win32/Win32Window.h"
#endif
#include "Input.h"

namespace Psio
{
	std::unique_ptr<Window> Window::createWindow(int width, int height)
	{
#ifdef WIN32
		std::unique_ptr<Window> win = std::make_unique<Win32Window>(width, height);		
#else
		// TODO
		Window *win = nullptr;
#endif
		return win;
	}

	void Window::setInput(Input* input)
	{
		m_input = input;
	}

	void Window::addInputEvent(std::shared_ptr<InputEvent> evt)
	{
#ifdef WIN32
		m_input->addEvent(evt);
#endif
	}

	void Window::onKeyDown(unsigned int key)
	{
		
	}
}