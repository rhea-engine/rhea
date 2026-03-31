#ifndef HIPPIOS_WGLCONTEXT_H
#define HIPPIOS_WGLCONTEXT_H

#include "GlContext.h"
#include "GLIncludes.h"

namespace Hippios
{
	class WglContext : public GlContext
	{
	public:
		WglContext();
		~WglContext() override;

		bool prepareSwapChain(SwapChain* swapChain) override;

		bool create(SwapChain* swapChain) override;
		void destroy() override;

		bool isCreated() const override;

		bool makeCurrent(SwapChain* swapChain = nullptr) override;
		void doneCurrent() override;
		void swapBuffers(SwapChain* swapChain = nullptr) override;

	private:
		bool setupPixelFormat(HDC hdc);
		bool createSharedContext(HDC hdc, HGLRC sharedContext);

	private:
		HGLRC m_glrc = nullptr;
	};
}

#endif // !HIPPIOS_WGLCONTEXT_H