#ifndef RHEA_GRAPHICS_MANAGER_H
#define RHEA_GRAPHICS_MANAGER_H

#include <Poseidon/Core.h>
#include <Poseidon/Node.h>
#include <RheaClient/Components/RenderComponent.h>

namespace rhea
{
	class Element;
	class Unit;
	class Material;
}

namespace rhea::client {
	class GraphicsService;

	/** base class for all managers
	*   in the graphics system. managers are subservices of the graphicsservice
	* 
	*   some utility functions for all renderables
	*/	
	class GraphicsManager
	{
	public:
		GraphicsManager(GraphicsService* service) :
			m_service(service)
		{}
		virtual ~GraphicsManager() {}

		// Ensures this unit is ready for the graphics pipeline
		virtual bool Ensure(poseidon::Core* core, rhea::Unit* unit) = 0;

		// handle frame update
		virtual void onFrame() = 0;

		// handle transform update
		virtual void onTransformUpdate(rhea::Unit* unit) {};

		// renderables utility functions

		// create a renderable mesh
		poseidon::RenderableHandle createRenderable(RenderComponent* rc, poseidon::Core* core, rhea::Unit* item, poseidon::Mesh* mesh, std::string render_context_name);
		poseidon::CameraHandle createCamera(poseidon::Core* core);
		poseidon::LightHandle createLight(poseidon::Core* core);
		
		// helper to find render context by name
		poseidon::RenderContext* getRenderContext(poseidon::Core* core, std::string render_context_name);
		
		// set position from values to transform t
		void setPosition(rhea::Unit* u, rhea::Transform* t);	
	
	protected:
		GraphicsService* m_service;
	};
}

#endif // !RHEA_GRAPHICS_MANAGER_H
