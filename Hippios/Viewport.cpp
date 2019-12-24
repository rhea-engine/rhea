#include "Camera.h"
#include "Viewport.h"

namespace Hippios
{
	Viewport::Viewport(Camera* camera, HippiosWindow* win, RenderListener* render_listener) : m_camera(camera), m_win(win), m_render_listener(render_listener)
	{
	}

	void Viewport::render()
	{
		m_render_listener->render(m_camera);
	}
}