#ifndef HIPPIOS_RENDER_LISTENER_H
#define HIPPIOS_RENDER_LISTENER_H

namespace Hippios
{
	class Camera;

	class RenderListener
	{
	public:
		virtual bool render(Camera* camera) = 0;
	};
}
#endif // !HIPPIOS_RENDER_LISTENER_H
