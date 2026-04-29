/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#ifndef HIPPIOS_SDLGLCONTEXT_H
#define HIPPIOS_SDLGLCONTEXT_H

#include "GLContext.h"
#include <SDL3/SDL.h>

namespace hippios
{
	class GLRenderer;

	class SdlGlContext : public GlContext
	{
	public:
		SdlGlContext(GLRenderer* renderer);
		~SdlGlContext() override;

		bool prepareSwapChain(SwapChain* swapChain) override;
		bool create(SwapChain* swapChain) override;
		void destroy() override;

		bool isCreated() const override;

		bool makeCurrent(SwapChain* swapChain = nullptr) override;
		void doneCurrent() override;
		void swapBuffers(SwapChain* swapChain = nullptr) override;

	private:
		GLRenderer* m_renderer = nullptr;
		SDL_GLContext m_sdl_context = nullptr;
	};
}

#endif