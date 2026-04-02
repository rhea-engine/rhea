#include <glad/glad.h>
#include "SdlContext.h"
#include "GLSwapChainSDL.h"
#include <Psio/Log.h>

namespace Hippios
{
	SdlGlContext::SdlGlContext()
	{
	}

	SdlGlContext::~SdlGlContext()
	{
		destroy();
	}

	bool SdlGlContext::prepareSwapChain(SwapChain* swapChain)
	{
		if (!swapChain)
			return false;

		auto* sc = static_cast<GLSwapChainSDL*>(swapChain);
		SDL_Window* window = sc->getWindow();
		if (!window)
			return false;

		SDL_WindowFlags flags = SDL_GetWindowFlags(window);
		if (!(flags & SDL_WINDOW_OPENGL))
		{
			LOG(LOG_ERROR) << "SDL window was not created with SDL_WINDOW_OPENGL flag!";
			return false;
		}
/*
		// Usually these attributes should ideally be set before window creation,
		// but keeping this here preserves your current abstraction shape.
		SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
		SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
		SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
		SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
		SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
		SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
		SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 5);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

#ifdef _DEBUG
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG);
#endif
*/
		return true;
	}

	bool SdlGlContext::create(SwapChain* swapChain)
	{
		if (m_sdl_context)
			return true;

		if (!swapChain)
			return false;

		auto* sc = static_cast<GLSwapChainSDL*>(swapChain);
		SDL_Window* window = sc->getWindow();
		if (!window)
			return false;

		m_sdl_context = SDL_GL_CreateContext(window);
		if (!m_sdl_context)
		{
			LOG(LOG_ERROR) << "SDL_GL_CreateContext failed: " << SDL_GetError();
			return false;
		}

		if (!SDL_GL_MakeCurrent(window, m_sdl_context))
		{
			LOG(LOG_ERROR) << "SDL_GL_MakeCurrent failed: " << SDL_GetError();
			SDL_GL_DestroyContext(m_sdl_context);
			m_sdl_context = nullptr;
			return false;
		}

		if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress))
		{
			LOG(LOG_ERROR) << "Failed to initialize GLAD with SDL";
			SDL_GL_MakeCurrent(window, nullptr);
			SDL_GL_DestroyContext(m_sdl_context);
			m_sdl_context = nullptr;
			return false;
		}

		int drawableW = 0;
		int drawableH = 0;
		if (!SDL_GetWindowSizeInPixels(window, &drawableW, &drawableH))
		{
			LOG(LOG_ERROR) << "SDL_GetWindowSizeInPixels failed: " << SDL_GetError();
			// Fallback oder Fehlerbehandlung
		}
		glViewport(0, 0, drawableW, drawableH);

		LOG(LOG_INFO) << "GL_VENDOR   = " << (const char*)glGetString(GL_VENDOR);
		LOG(LOG_INFO) << "GL_RENDERER = " << (const char*)glGetString(GL_RENDERER);
		LOG(LOG_INFO) << "GL_VERSION  = " << (const char*)glGetString(GL_VERSION);

		GLint currentFbo = 0;
		glGetIntegerv(GL_DRAW_FRAMEBUFFER_BINDING, &currentFbo);

		GLint currentVao = 0;
		glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &currentVao);

		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LESS);
		glFrontFace(GL_CW);		

		return true;
	}

	void SdlGlContext::destroy()
	{
		if (m_sdl_context)
		{
			SDL_GL_DestroyContext(m_sdl_context);
			m_sdl_context = nullptr;
		}
	}

	bool SdlGlContext::isCreated() const
	{
		return m_sdl_context != nullptr;
	}

	bool SdlGlContext::makeCurrent(SwapChain* swapChain)
	{
		if (!swapChain || !m_sdl_context)
			return false;

		auto* sc = static_cast<GLSwapChainSDL*>(swapChain);
		SDL_Window* window = sc->getWindow();
		if (!window)
			return false;

		if (!SDL_GL_MakeCurrent(window, m_sdl_context))
		{
			LOG(LOG_ERROR) << "SDL_GL_MakeCurrent failed: " << SDL_GetError();
			return false;
		}

		return true;
	}

	void SdlGlContext::doneCurrent()
	{
		SDL_GL_MakeCurrent(nullptr, nullptr);
	}

	void SdlGlContext::swapBuffers(SwapChain* swapChain)
	{
		if (!swapChain || !m_sdl_context)
			return;

		auto* sc = static_cast<GLSwapChainSDL*>(swapChain);
		SDL_Window* window = sc->getWindow();
		if (!window)
			return;

		if (!SDL_GL_SwapWindow(window))
		{
			LOG(LOG_ERROR) << "SDL_GL_SwapWindow failed: " << SDL_GetError();
		}
	}
}