#ifndef GL_SWAPCHAIN_H
#define GL_SWAPCHAIN_H

#include <Hippios/SwapChain.h>
#include "GLIncludes.h"

namespace Hippios
{
	class GLSwapChain : public SwapChain
	{
	public:
		GLSwapChain(HDC hdc);
		~GLSwapChain();

		void onResize(int width, int height) override;
		void SetFullscreen(bool mode) override;

		HDC getHDC() const { return m_hdc; }

	private:
		HDC m_hdc;
	};
}
#endif // !GL_SWAPCHAIN_H