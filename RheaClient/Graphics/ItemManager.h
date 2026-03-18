#ifndef RHEA_GRAPHICS_ITEM_H
#define RHEA_GRAPHICS_ITEM_H

#include <Poseidon/Core.h>
#include <Poseidon/Node.h>
#include <Poseidon/RenderContext.h>

#include <Rhea/Element.h>
#include <Rhea/Graphics/GraphicsManager.h>

namespace Rhea
{
	class Unit;
	class Item;

	class GraphicsItemManager : public GraphicsManager
	{
	public:
		GraphicsItemManager(GraphicsService* service) :
			GraphicsManager(service)
		{}
		~GraphicsItemManager();

		// create an element for this unit
		bool Ensure(Poseidon::Core* core, Unit* unit);

		// handle update event
		bool onUpdate(Event* ev, Unit* unit);

		// handle frame update
		void onFrame() {};

		// handle deletion event
		bool onItemDelete(Event* ev, Unit* unit);
	};
}

#endif // !RHEA_GRAPHICS_ITEM_H
