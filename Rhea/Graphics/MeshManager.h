#ifndef RHEA_GPAPHICS_MESHMANAGER_H
#define RHEA_GPAPHICS_MESHMANAGER_H

#include "GraphicsManager.h"
#include "MeshElement.h"

namespace Rhea
{
	class GraphicsMeshManager : public GraphicsManager
	{
	public:
		GraphicsMeshManager(GraphicsService* service) :
			GraphicsManager(service)
		{}

		// create an element for this unit
		Element* createElement(Poseidon::Core* core, Unit* unit);

		// handle frame update
		void onFrame() {}

		// handle events
		bool onUpdate(Event* ev, Unit* unit);
		bool onDelete(Event* ev, Unit* unit);

	protected:
		std::vector<GraphicsMesh*> m_elements;

		bool ensureMaterialElement(Material* material);
	};
}

#endif // !RHEA_GPAPHICS_MESHMANAGER_H
