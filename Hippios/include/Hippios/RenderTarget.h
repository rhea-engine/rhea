#ifndef HIPPIOS_RENDERTARGET_H
#define HIPPIOS_RENDERTARGET_H

#include "BaseTypes.h"

namespace hippios
{
	class Renderer;
	class SwapChain;

	enum class RenderTargetType
	{
		SwapChain,		// backbuffer from swap chain
		Texture			// offscreen texture render target
	};

	struct RenderTargetDesc
	{
		RenderTargetType type = RenderTargetType::SwapChain;
		int width = 0;
		int height = 0;
		int depth_bits = 24;		// 0 = no depth buffer, 16, 24, 32
		SwapChain* swap_chain = nullptr;	// required for SwapChain type
		// future: format, sample count, etc.
	};

	class RenderTarget
	{
	public:
		virtual ~RenderTarget() {}

		virtual bool create(Renderer* renderer, const RenderTargetDesc& desc) = 0;

		// recreate with new dimensions (e.g. on resize)
		virtual bool resize(int width, int height) = 0;

		virtual void Release() = 0;
		virtual void setActive() = 0;
		virtual void clearPass() = 0;
		virtual void setViewport(ViewportDesc* desc) = 0;
	};
}

#endif // !HIPPIOS_RENDERTARGET_H
