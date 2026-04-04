#include "GLSwapChainWin32.h"
#include <Psio/Log.h>

namespace hippios
{
	GLSwapChainWin32::GLSwapChainWin32(NativeWindow* win) :
		m_native_win_handle(win->native_win_handle),
		SwapChain()
	{
		HWND hwnd = static_cast<HWND>(win->native_win_handle);
		if (hwnd)
			m_hdc = ::GetDC(hwnd);
		else
			m_hdc = nullptr;

		if (!m_hdc)
			LOG(LOG_ERROR) << "GLSwapChainWin32: GetDC failed";
	}

	GLSwapChainWin32::~GLSwapChainWin32()
	{
		if (m_hdc && m_native_win_handle)
			ReleaseDC((HWND)m_native_win_handle, m_hdc);
	}

	void GLSwapChainWin32::onResize(int width, int height)
	{
		glViewport(0, 0, width, height);
		
		m_width = width;
		m_height = height;
	}

	void GLSwapChainWin32::SetFullscreen(bool mode)
	{
		// Fullscreen toggle is handled at the window level (Win32/GLFW)
	}

	bool GLSwapChainWin32::isValid() const
	{
		return m_native_win_handle != nullptr && m_hdc != nullptr;
	}
}