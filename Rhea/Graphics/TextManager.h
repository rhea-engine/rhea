#ifndef RHEA_GRAPHICS_TEXTMGR_H
#define RHEA_GRAPHICS_TEXTMGR_H

#include <vector>
#include "GraphicsManager.h"

namespace Rhea
{
	class GraphicsRenderElement;
	class TextWidget;

	class GraphicsTextManager : public GraphicsManager
	{
	public:
		GraphicsTextManager(GraphicsService* service) :
			GraphicsManager(service)
		{}

		// create an element for this unit
		Element* createElement(Poseidon::Core* core, Unit* unit);

		// set the text to a new value
		void setText(TextWidget* t, GraphicsRenderElement* elem);

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

#endif // !RHEA_GRAPHICS_TEXTMGR_H
