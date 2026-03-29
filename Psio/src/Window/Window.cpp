#include <Psio/Window/Window.h>
#ifdef WIN32
#  include "../Platform/Win32/Window/Win32Window.h"
#endif
#include <Psio/Input/Input.h>

namespace Psio
{
	std::unique_ptr<Window> Window::createWindow(std::string name, int width, int height)
	{
#ifdef WIN32
		std::unique_ptr<Window> win = std::make_unique<Win32Window>(name, width, height);		
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