#ifndef HIPPIOS_VIEWPORT_H
#define HIPPIOS_VIEWPORT_H

#include "RenderListener.h"

namespace Hippios
{
	class Camera;
	class HippiosWindow;

	class Viewport
	{
	protected:
		Camera* m_camera;
		HippiosWindow* m_win;
		RenderListener* m_render_listener;
	public:
		Viewport(Camera* camera, HippiosWindow *win, RenderListener *render_listener);
		void render();
		Camera* get_camera() { return m_camera; };
	};
}

#endif # !HIPPIOS_VIEWPORT_H