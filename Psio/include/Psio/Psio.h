#ifndef PSIO_H
#define PSIO_H

#include <memory>
#include <cstdint>
#include <string>

namespace Psio
{
	class Input;
	class InputEvent;
	class Window;
	class Network;
	class IO;
	class Fs;
	class IBackend;

	enum class BackendKind
	{
		Auto,
		Win32,
		Unix,
		SDL
	};

	enum class WindowOption
	{
		None = 0,
		OpenGL = 1,
		Vulkan = 2,
		Fullscreen = 4,
	};

	inline WindowOption operator|(WindowOption a, WindowOption b)
	{
		return static_cast<WindowOption>(
			static_cast<int>(a) | static_cast<int>(b)
			);
	}

	inline WindowOption operator&(WindowOption a, WindowOption b)
	{
		return static_cast<WindowOption>(
			static_cast<int>(a) & static_cast<int>(b)
			);
	}

	inline WindowOption& operator|=(WindowOption& a, WindowOption b)
	{
		a = a | b;
		return a;
	}

	inline bool hasFlag(WindowOption value, WindowOption flag)
	{
		return (value & flag) == flag;
	}

	class Engine
	{
	public:
		Engine(BackendKind backend = BackendKind::Auto);
		~Engine();

		// create a window
		std::unique_ptr <Window> createWindow(std::string name, int width, int height, WindowOption options);

		// get input Handler
		Input* getInput() { return m_input.get(); }

		// iterate the inout system once, this will update the input state and generate input events
		void iterateInput();

		// get the next input event
		std::shared_ptr<InputEvent> pollInput();

		// run network handling
		void RunNet();

		// get network handler
		Network* getNet() { return m_net.get(); }

		// get IO subsys
		IO* getIO() { return m_io.get(); }

		// set alternative network handler
		void setNetwork(Network* network);

		// get FS subsystem
		Fs* getFs() { return m_fs.get(); }

		// Returns the system time, in microseconds
		uint64_t getTicks();

		// return the kind of backend in use
		const char* getBackendName();

	protected:
		std::unique_ptr<IBackend> m_backend;
		std::unique_ptr<Input> m_input;
		std::unique_ptr<Network> m_net;
		std::unique_ptr<IO> m_io;
		std::unique_ptr<Fs> m_fs;
	};
}
#endif // !PSIO_H
