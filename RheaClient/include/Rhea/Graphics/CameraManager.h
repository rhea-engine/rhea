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
		bool Ensure(Poseidon::Core* core, rhea::Unit* unit);

		// handle update event
		bool onUpdate(rhea::Event* ev, rhea::Unit* unit);

		// handle frame update
		void onFrame();

		// handle events
		bool onItemDelete(rhea::Event* ev, rhea::Unit* unit);

	protected:		
	};
}
#endif // !RHEA_GRAPHICS_CAMERA_H
