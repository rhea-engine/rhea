#ifndef PSIO_H
#define PSIO_H

#include <memory>
#include <cstdint>

namespace Psio
{
	class Input;
	class InputEvent;
	class Window;
	class Network;
	class IO;

	class Engine
	{
	public:
		Engine();

		// create a window
		Window* createWindow();

		// get input Handler
		Input* getInput() { return m_input; }

		// get the next input event
		std::shared_ptr<InputEvent> pollInput();

		// run network handling
		void RunNet();

		// get network handler
		Network* getNet() { return m_net; }

		// get IO subsys
		IO* getIO() { return m_io; }

		// set alternative network handler
		void setNetwork(Network* network);

		// Returns the system time, in microseconds
		uint64_t getTicks();

	protected:
		Input* m_input;
		Network* m_net;
		IO* m_io;
	};
}
#endif // !PSIO_H
