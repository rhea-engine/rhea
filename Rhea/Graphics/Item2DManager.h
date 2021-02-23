#ifndef RHEA_GRAPHICS_Item2D_H
#define RHEA_GRAPHICS_Item2D_H

#include <Poseidon/Core.h>
#include <Poseidon/Node.h>
#include <Poseidon/RenderContext.h>

#include "GraphicsManager.h"

namespace Rhea
{
	class Unit;
	class Item2D;

	class GraphicsItem2DManager : public GraphicsManager
	{
	public:
		GraphicsItem2DManager(GraphicsService* service) :
			GraphicsManager(service)
		{}

		Element* createElement(Poseidon::Core* core, Unit* unit);

		// set the item to a new value
		void setItem2D(Item2D* t, GraphicsRenderElement* elem);

		// handle update event
		bool onUpdate(Event* ev, Unit* unit);

		// handle frame update
		void onFrame();

		// handle events
		bool onItemDelete(Event* ev, Unit* unit);

	protected:
		std::vector<GraphicsRenderElement*> m_elements;
	};
}

#endif // !RHEA_GRAPHICS_ITEM_H
