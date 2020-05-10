// Poseidon simple input output
#include "Psio.h"
#include "InputEvent.h"
#include "Input.h"
#include "Window.h"
#include "Keyboard.h"
#include "Timer.h"

namespace Psio
{
	Engine::Engine()
	{
		m_input = new Input();
		initHwKey();
	}

	std::shared_ptr<InputEvent> Engine::pollInput()
	{
		return m_input->poll();
	}

	uint64_t Engine::getTicks()
	{
		return Timer::getTicks();
	}

	Window* Engine::createWindow()
	{
		Window* win = Window::createWindow();
		win->setInput(m_input);
		return win;
	}
}