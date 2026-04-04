#ifndef RHEA_GRAPHICS_CAMERA_H
#define RHEA_GRAPHICS_CAMERA_H

#include <Rhea/Event.h>
#include "GraphicsManager.h"

namespace Poseidon
{
	class Core;
	class Camera;
	class Viewport;
}

namespace rhea::client {
	class Camera;
	
	class GraphicsCameraManager : public GraphicsManager
	{
	public:
		GraphicsCameraManager(GraphicsService* service) :
			GraphicsManager(service)
		{}

		// factory function
		bool Ensure(Poseidon::Core* core, Rhea::Unit* unit);

		// handle update event
		bool onUpdate(Rhea::Event* ev, Rhea::Unit* unit);

		// handle frame update
		void onFrame();

		// handle events
		bool onItemDelete(Rhea::Event* ev, Rhea::Unit* unit);

	protected:		
	};
}
#endif // !RHEA_GRAPHICS_CAMERA_H
