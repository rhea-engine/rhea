#include "Window.h"

namespace Poseidon
{
	void Window::initRenderTarget(Hippios::Engine* engine)
	{		
		m_win = engine->createWindow();
		Viewport::initRenderTarget(engine);
	}
}
