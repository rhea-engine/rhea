#ifndef RHEA_GRAPHICS_ITEM_H
#define RHEA_GRAPHICS_ITEM_H

#include <Poseidon/Core.h>
#include <Poseidon/Node.h>
#include <Poseidon/RenderContext.h>

#include <Rhea/Element.h>
#include <RheaClient/Graphics/GraphicsManager.h>

namespace rhea {
	class Event;
	class Unit;
	class Item;
}
namespace rhea::client {
	
	class GraphicsItemManager : public GraphicsManager
	{
	public:
		GraphicsItemManager(GraphicsService* service) :
			GraphicsManager(service)
		{}
		~GraphicsItemManager();

		// create an element for this unit
		bool Ensure(poseidon::Core* core, rhea::Unit* unit);

		// handle update event
		bool onUpdate(rhea::Event* ev, rhea::Unit* unit);

		// handle transform update
		void onTransformUpdate(rhea::Unit* unit) override;

		// handle frame update
		void onFrame() {};

		// handle deletion event
		bool onItemDelete(rhea::Event* ev, rhea::Unit* unit);

	private:
		bool createGPUResources(poseidon::Core* core, 
		                        rhea::Item* item,
		                        RenderComponent* rc, 
		                        rhea::Mesh* mesh);
		bool syncRenderable(poseidon::Core* core, rhea::Item* item, RenderComponent* rc);
		bool syncTransform(poseidon::Core* core, rhea::Item* item, RenderComponent* rc);
	};
}

#endif // !RHEA_GRAPHICS_ITEM_H
