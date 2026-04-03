#ifndef HIPPIOS_GLSWAPCHAINWIN32_H
#define HIPPIOS_GLSWAPCHAINWIN32_H

#include <Hippios/SwapChain.h>
#include <Hippios/BaseTypes.h>
#include "GLIncludes.h"

namespace Hippios
{
	class GLSwapChainWin32 : public SwapChain
	{
	public:
		GLSwapChainWin32(NativeWindow* win);
		~GLSwapChainWin32();

		bool isValid() const override;

		void onResize(int width, int height) override;
		void SetFullscreen(bool mode) override;

		HDC getHDC() const { return m_hdc; }
		void* getWindowHandle() const { return m_native_win_handle; }

	protected:
		HDC m_hdc = nullptr;
		void* m_native_win_handle = nullptr;
	};
}
#endif // !HIPPIOS_GLSWAPCHAINWIN32_H