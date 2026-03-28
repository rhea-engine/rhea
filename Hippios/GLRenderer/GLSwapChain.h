#ifndef GL_SWAPCHAIN_H
#define GL_SWAPCHAIN_H

#include <Hippios/SwapChain.h>
#include <Hippios/BaseTypes.h>
#include "GLIncludes.h"

namespace Hippios
{
	class GLSwapChain : public SwapChain
	{
	public:
		GLSwapChain(HDC hdc, NativeWindow* win);
		~GLSwapChain();

		void onResize(int width, int height) override;
		void SetFullscreen(bool mode) override;

		HDC getHDC() const { return m_hdc; }
		void* getWindowHandle() const { return m_native_win_handle; }

	private:
		HDC m_hdc;
		void* m_native_win_handle;		
	};
}
#endif // !GL_SWAPCHAIN_H