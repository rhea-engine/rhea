#ifndef RHEA_GRAPHICS_CAMERA_H
#define RHEA_GRAPHICS_CAMERA_H

#include "../Event.h"
#include "../Element.h"
#include "GraphicsManager.h"

namespace Poseidon
{
	class Core;
	class Camera;
	class Viewport;
}

namespace Rhea
{
	class Camera;
	
	class GraphicsCameraManager : public GraphicsManager
	{
	public:
		GraphicsCameraManager(GraphicsService* service) :
			GraphicsManager(service)
		{}

		// factory function
		Element* createElement(Poseidon::Core* core, Unit* unit);

		// handle update event
		bool onUpdate(Event* ev, Unit* unit);

		// handle frame update
		void onFrame();

		// handle events
		bool onItemDelete(Event* ev, Unit* unit);

	protected:		
	};
}
#endif // !RHEA_GRAPHICS_CAMERA_H
