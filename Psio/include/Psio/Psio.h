/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#ifndef PSIO_H
#define PSIO_H

#include <memory>
#include <cstdint>
#include <string>

namespace psio
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

	struct WindowDesc
	{
		int width;
		int height;
		std::string name;
		bool fullscreen;
		struct backend {
			bool opengl;
			bool vulkan;
			int major_version;
			int minor_version;
			bool core_profile; // for opengl
			bool debug_context; // for opengl
		} backend;
	};	

	class Engine
	{
	public:
		Engine(BackendKind backend = BackendKind::Auto);
		~Engine();

		// create a window
		std::unique_ptr <Window> createWindow(WindowDesc desc);

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
