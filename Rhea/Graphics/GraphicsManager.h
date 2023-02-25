#ifndef RHEA_GRAPHICS_MANAGER_H
#define RHEA_GRAPHICS_MANAGER_H

#include <Poseidon/Core.h>
#include <Poseidon/Node.h>

namespace Rhea
{
	class Element;
	class Unit;
	class GraphicsService;
	class Material;
	class GraphicsRenderElement;

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

		// create an element for this unit
		virtual Element* createElement(Poseidon::Core* core, Unit* unit) = 0;

		// handle frame update
		virtual void onFrame() = 0;

		// renderables utility functions

		// create a renderable mesh
		Poseidon::Node* createRenderable(GraphicsRenderElement* elem, Poseidon::Core* core, Unit* item, std::string render_context_name, bool dynamic_usage);
		
		// remove renderable from engine
		void removeRenderable(Item* elem);

		// set position from values to transform t
		void setPosition(Unit* u, Transform* t);

		// set material
		void setMaterial(Poseidon::Core* core, Material* material);

		// default event functions
		bool onHide(Event* ev, Unit* unit);
		bool onShow(Event* ev, Unit* unit);

	protected:
		GraphicsService* m_service;
	};
}

#endif // !RHEA_GRAPHICS_MANAGER_H
