#ifndef RHEA_GRAPHICS_CAMERA_H
#define RHEA_GRAPHICS_CAMERA_H

#include "../Element.h"
#include "Item.h"

namespace Poseidon
{
	class Core;
	class Camera;
	class Viewport;
}

namespace Rhea
{
	class Camera;
	
	class GraphicsCamera : public GraphicsItem
	{
	public:
		GraphicsCamera(Poseidon::Core* core) :			
			m_viewport(NULL),
			GraphicsItem(core)
		{}

		// create camera node
		void createCamera(Camera* cam);

		// factory function
		static Element* createElement(Poseidon::Core* core, Unit* unit);

	protected:
		Poseidon::Viewport* m_viewport;		
	};
}
#endif // !RHEA_GRAPHICS_CAMERA_H
