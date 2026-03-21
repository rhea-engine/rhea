#include "GLSwapChain.h"

namespace Hippios
{
	GLSwapChain::GLSwapChain(HDC hdc) :
		m_hdc(hdc)
	{
	}

	GLSwapChain::~GLSwapChain()
	{
	}

	void GLSwapChain::onResize(int width, int height)
	{
		glViewport(0, 0, width, height);
	}

	void GLSwapChain::SetFullscreen(bool mode)
	{
		// Fullscreen toggle is handled at the window level (Win32/GLFW)
	}
}