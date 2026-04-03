#ifndef HIPPIOS_WGLCONTEXT_H
#define HIPPIOS_WGLCONTEXT_H

#include "GLContext.h"
#include "GLIncludes.h"

namespace Hippios
{
	class GLRenderer;

	class WglContext : public GlContext
	{
	public:
		WglContext(GLRenderer* renderer);
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
		GLRenderer* m_renderer = nullptr;
		HGLRC m_glrc = nullptr;
	};
}

#endif // !HIPPIOS_WGLCONTEXT_H