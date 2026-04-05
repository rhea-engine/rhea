#include <glad/glad.h>
#include <Hippios/BaseTypes.h>
#include "GLSwapChainSDL.h"
#include <Psio/Log.h>

namespace hippios
{
	GLSwapChainSDL::GLSwapChainSDL(NativeWindow* win)
		: SwapChain()
	{
		if (win)
			m_window = static_cast<SDL_Window*>(win->native_win_handle);

		if (!m_window)
			LOG(LOG_ERROR) << "GLSwapChainSDL: invalid SDL window";

		int drawableW = 0;
		int drawableH = 0;
		if (!SDL_GetWindowSizeInPixels(m_window, &drawableW, &drawableH))
		{
			LOG(LOG_ERROR) << "SDL_GetWindowSizeInPixels failed: " << SDL_GetError();
			return;
		}

		m_width = drawableW;
		m_height = drawableH;
	}

	bool GLSwapChainSDL::isValid() const
	{
		return m_window != nullptr;
	}

	void GLSwapChainSDL::onResize(int width, int height)
	{
		int drawableW = 0;
		int drawableH = 0;
		if (!SDL_GetWindowSizeInPixels(m_window, &drawableW, &drawableH))
		{
			LOG(LOG_ERROR) << "SDL_GetWindowSizeInPixels failed: " << SDL_GetError();
			// Fallback oder Fehlerbehandlung
		}
		glViewport(0, 0, drawableW, drawableH);

		m_width = drawableW;
		m_height = drawableH;
	}

	void GLSwapChainSDL::SetFullscreen(bool mode)
	{
		if (!m_window)
			return;
		SDL_SetWindowFullscreen(m_window, mode);
	}
}