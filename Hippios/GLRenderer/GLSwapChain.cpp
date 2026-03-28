#include "GLSwapChain.h"

namespace Hippios
{
	GLSwapChain::GLSwapChain(HDC hdc, NativeWindow* win) :
		m_hdc(hdc),
		m_native_win_handle(win->native_win_handle)
	{
	}

	GLSwapChain::~GLSwapChain()
	{
		if (m_hdc)
			ReleaseDC((HWND)m_native_win_handle, m_hdc);
	}

	void GLSwapChain::onResize(int width, int height)
	{
		glViewport(0, 0, width, height);
		
		m_width = width;
		m_height = height;
	}

	void GLSwapChain::SetFullscreen(bool mode)
	{
		// Fullscreen toggle is handled at the window level (Win32/GLFW)
	}
}