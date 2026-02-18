#ifndef RHEA_GRAPHICS_MANAGER_H
#define RHEA_GRAPHICS_MANAGER_H

#include <Poseidon/Core.h>
#include <Poseidon/Node.h>
#include <Rhea/Components/RenderComponent.h>

namespace Rhea
{
	class Element;
	class Unit;
	class GraphicsService;
	class Material;

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
		virtual bool Ensure(Poseidon::Core* core, Unit* unit) = 0;

		// handle frame update
		virtual void onFrame() = 0;

		// renderables utility functions

		// create a renderable mesh
		Poseidon::Node* createRenderable(RenderComponent* rc, Poseidon::Core* core, Unit* item, Poseidon::Mesh* mesh, std::string render_context_name);
		
		// create an entity
		Poseidon::Node* createEntity(RenderComponent* rc, Poseidon::Core* core, Unit* item, std::string render_context_name, bool dynamic_usage);

		// set position from values to transform t
		void setPosition(Unit* u, Transform* t);		

	protected:
		GraphicsService* m_service;
	};
}

#endif // !RHEA_GRAPHICS_MANAGER_H
