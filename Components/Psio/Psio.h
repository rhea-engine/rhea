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
	class Fs;

	class Engine
	{
	public:
		Engine();

		// create a window
		std::unique_ptr <Window> createWindow(int width, int height);

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

		// get FS subsystem
		Fs* getFs() { return m_fs; }

		// Returns the system time, in microseconds
		uint64_t getTicks();

	protected:
		Input* m_input;
		Network* m_net;
		IO* m_io;
		Fs* m_fs;
	};
}
#endif // !PSIO_H
