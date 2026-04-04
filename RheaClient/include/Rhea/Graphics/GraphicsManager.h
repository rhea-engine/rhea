#ifndef RHEA_GRAPHICS_MANAGER_H
#define RHEA_GRAPHICS_MANAGER_H

#include <Poseidon/Core.h>
#include <Poseidon/Node.h>
#include <Rhea/Components/RenderComponent.h>

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
		virtual bool Ensure(Poseidon::Core* core, rhea::Unit* unit) = 0;

		// handle frame update
		virtual void onFrame() = 0;

		// renderables utility functions

		// create a renderable mesh
		Poseidon::Node* createRenderable(RenderComponent* rc, Poseidon::Core* core, rhea::Unit* item, Poseidon::Mesh* mesh, std::string render_context_name);
		
		// create an entity
		Poseidon::Node* createEntity(RenderComponent* rc, Poseidon::Core* core, rhea::Unit* item, std::string render_context_name, bool dynamic_usage);

		// set position from values to transform t
		void setPosition(rhea::Unit* u, rhea::Transform* t);	
	
	protected:
		GraphicsService* m_service;
	};
}

#endif // !RHEA_GRAPHICS_MANAGER_H
