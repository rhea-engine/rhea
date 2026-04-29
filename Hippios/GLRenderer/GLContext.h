/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#ifndef HIPPIOS_GL_CONTEXT_H
#define HIPPIOS_GL_CONTEXT_H

#include <memory>

namespace hippios
{
	struct NativeWindow;
	class SwapChain;

	class GlContext
	{
	public:
		virtual ~GlContext() = default;

		virtual bool prepareSwapChain(SwapChain* swapChain) = 0;

		virtual bool create(SwapChain* swapChain) = 0;
		virtual void destroy() = 0;

		virtual bool isCreated() const = 0;

		virtual bool makeCurrent(SwapChain* swapChain = nullptr) = 0;
		virtual void doneCurrent() = 0;
		virtual void swapBuffers(SwapChain* swapChain = nullptr) = 0;
	};
}

#endif // !HIPPIOS_GL_CONTEXT_H