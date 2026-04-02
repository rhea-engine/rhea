// Poseidon simple input output
#include <Psio/Psio.h>
#include <Psio/Input/InputEvent.h>
#include <Psio/Input/Input.h>
#include <Psio/Window/Window.h>
#include <Psio/Input/Keyboard.h>
#include <Psio/Timer.h>
#include <Psio/Net/Network.h>
#include <Psio/IPC/IO.h>
#include <Psio/Fs/Fs.h>
#include <stdexcept>
#include "Backend/IBackend.h"
#ifdef PSIO_HAS_WIN32
#include "Platform/Win32/Win32Backend.h"
#endif 
#ifdef PSIO_HAS_UNIX
#include "Platform/Unix/UnixBackend.h"
#endif 
#ifdef PSIO_HAS_SDL
#include "Platform/SDL/SDLBackend.h"
#endif
#include <Psio/Log.h>

namespace Psio
{
	std::unique_ptr<IBackend> CreateBackend(BackendKind preferred)
	{
		if (preferred == BackendKind::SDL)
		{
#ifdef PSIO_HAS_SDL
			LOG(LOG_INFO) << "Using SDL backend as requested.";
			return std::make_unique<SDLBackend>();
#else
			throw std::runtime_error("SDL backend requested but not available.");
#endif
		}

		if (preferred == BackendKind::Win32)
		{
#ifdef PSIO_HAS_WIN32
			LOG(LOG_INFO) << "Using Win32 backend as requested.";
			return std::make_unique<Win32Backend>();
#else
			throw std::runtime_error("Win32 backend requested but not available.");
#endif
		}

		if (preferred == BackendKind::Unix)
		{
#ifdef PSIO_HAS_UNIX
			LOG(LOG_INFO) << "Using Unix backend as requested.";
			return std::make_unique<UnixBackend>();
#else
			throw std::runtime_error("Unix backend requested but not available.");
#endif
		}

		// Auto mode: choose best available for this platform
#ifdef PSIO_HAS_WIN32
		LOG(LOG_INFO) << "Auto-selecting Win32 backend.";
		return std::make_unique<Win32Backend>();
#elif defined(PSIO_HAS_UNIX)
		LOG(LOG_INFO) << "Auto-selecting Unix backend.";
		return std::make_unique<UnixBackend>();
#elif defined(PSIO_HAS_SDL)
		LOG(LOG_INFO) << "Auto-selecting SDL backend.";
		return std::make_unique<SDLBackend>();
#else
		throw std::runtime_error("No supported Psio backend available.");
#endif
	}

	Engine::Engine(BackendKind backend)
	{
		m_backend = CreateBackend(backend);

		m_input = m_backend->CreateInput(this);
		m_net = m_backend->CreateNetwork(this);
		if (m_net)
		{
			m_net->Init();
		}
		m_io = m_backend->CreateIO(this);
		m_fs = m_backend->CreateFs(this);
	}

	Engine::~Engine()
	{
	}

	void Engine::setNetwork(Network* network)
	{
		m_net.reset(network);
	}

	void Engine::iterateInput()
	{
		if (m_input) {
			m_input->update();
		}
	}

	std::shared_ptr<InputEvent> Engine::pollInput()
	{
		if (!m_input) {
			return {};
		}
		return m_input->poll();
	}

	void Engine::RunNet()
	{
		m_net->RunLoop();
		m_io->RunLoop();
	}

	uint64_t Engine::getTicks()
	{
		return Timer::getTicks();
	}

	std::unique_ptr <Window> Engine::createWindow(std::string name, int width, int height, WindowOption options)
	{
		if (!m_backend) {
			throw std::runtime_error("Cannot create window: backend not initialized.");
		}

		std::unique_ptr<Window> window = m_backend->CreateNativeWindow(name, width, height, options);
		
		if (window) {
			window->setInput(m_input.get());
			window->create();
		}

		return window;
	}

	const char* Engine::getBackendName()
	{
		return m_backend ? m_backend->GetName() : "No Backend";
	}
}