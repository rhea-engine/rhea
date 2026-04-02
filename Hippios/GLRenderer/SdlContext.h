#ifndef HIPPIOS_SDLGLCONTEXT_H
#define HIPPIOS_SDLGLCONTEXT_H

#include "GlContext.h"
#include <SDL3/SDL.h>

namespace Hippios
{
	class SdlGlContext : public GlContext
	{
	public:
		SdlGlContext();
		~SdlGlContext() override;

		bool prepareSwapChain(SwapChain* swapChain) override;
		bool create(SwapChain* swapChain) override;
		void destroy() override;

		bool isCreated() const override;

		bool makeCurrent(SwapChain* swapChain = nullptr) override;
		void doneCurrent() override;
		void swapBuffers(SwapChain* swapChain = nullptr) override;

	private:
		SDL_GLContext m_sdl_context = nullptr;
	};
}

#endif