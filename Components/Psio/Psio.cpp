// Poseidon simple input output
#include "Psio.h"
#include "InputEvent.h"
#include "Input.h"
#include "Window.h"
#include "Keyboard.h"
#include "Timer.h"
#include "Net/Network.h"
#include "IPC/IO.h"
#include "Fs/Fs.h"

namespace Psio
{
	Engine::Engine()
	{
#ifdef WIN32
		m_input = new Input();
		initHwKey();
#endif
		m_net = new Network(this);
		m_net->Init();
		m_io = new IO(this);
		m_fs = new Fs("");
	}

	void Engine::setNetwork(Network* network)
	{
		if (m_net)
			delete m_net;
		m_net = network;
	}

	std::shared_ptr<InputEvent> Engine::pollInput()
	{
#ifdef WIN32
		return m_input->poll();
#endif
	}

	void Engine::RunNet()
	{
		m_net->RunLoop();
		m_io->RunLoop();
	}

	uint64_t Engine::getTicks()
	{
#ifdef WIN32
		return Timer::getTicks();
#endif
	}

	Window* Engine::createWindow()
	{
		Window* win = Window::createWindow();
		win->setInput(m_input);
		return win;
	}
}