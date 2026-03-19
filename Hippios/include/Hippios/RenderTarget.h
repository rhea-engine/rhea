#ifndef HIPPIOS_RENDERTARGET_H
#define HIPPIOS_RENDERTARGET_H

#include "BaseTypes.h"

namespace Hippios
{
	class Renderer;
	class SwapChain;

	class RenderTarget
	{
	public:
		virtual ~RenderTarget() {}

		virtual bool create(Renderer* renderer, int width, int height, SwapChain* swap_chain) = 0;

		// resize the render target to new width/height
		// also updates depth buffers
		virtual bool create(int width, int height, SwapChain* swap_chain) = 0;

		// Release the buffer
		virtual void Release() = 0;

		virtual void setActive() = 0;
		virtual void clearPass() = 0;
		virtual void setViewport(ViewportDesc* desc) = 0;
	};
}

#endif // !HIPPIOS_RENDERTARGET_H
