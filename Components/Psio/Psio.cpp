// Poseidon simple input output
#include "Psio.h"
#include "InputEvent.h"
#include "Input.h"
#include "Window.h"
#include "Keyboard.h"
#include "Timer.h"
#include "Net/Network.h"

namespace Psio
{
	Engine::Engine()
	{
		m_input = new Input();
		initHwKey();
		m_net = new Network(this);
		m_net->Init();
	}

	void Engine::setNetwork(Network* network)
	{
		if (m_net)
			delete m_net;
		m_net = network;
	}

	std::shared_ptr<InputEvent> Engine::pollInput()
	{
		return m_input->poll();
	}

	void Engine::RunNet()
	{
		m_net->RunLoop();
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