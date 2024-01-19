#ifndef RHEA_GRAPHICS_MATERIAL_MGR_H
#define RHEA_GRAPHICS_MATERIAL_MGR_H

#include <vector>
#include "GraphicsManager.h"

namespace Rhea
{
	class GraphicsMaterial : public Element
	{
	public:
		Poseidon::Material* m_material{};
	};

	class GraphicsMaterialManager : public GraphicsManager
	{
	public:
		GraphicsMaterialManager(GraphicsService* service) :
			GraphicsManager(service)
		{}

		// create an element for this unit
		Element* createElement(Poseidon::Core* core, Unit* unit);

		// handle frame update
		void onFrame() {}

		// handle events
		bool onItemDelete(Event* ev, Unit* unit);

	protected:
		std::vector<GraphicsMaterial*> m_elements;
	};
}

#endif // !RHEA_GRAPHICS_MATERIAL_MGR_H
