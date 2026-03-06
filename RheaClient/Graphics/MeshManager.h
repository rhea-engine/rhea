#ifndef RHEA_GPAPHICS_MESHMANAGER_H
#define RHEA_GPAPHICS_MESHMANAGER_H

#include "GraphicsManager.h"

#include <Poseidon/Core.h>
#include <Poseidon/Mesh.h>
#include <Rhea/Core/Mesh.h>

namespace Rhea
{
	class GraphicsMeshManager : public GraphicsManager
	{
	public:
		GraphicsMeshManager(GraphicsService* service) :
			GraphicsManager(service)
		{}

		// create an element for this unit
		bool Ensure(Poseidon::Core* core, Unit* unit);

		// handle frame update
		void onFrame() {}

		// handle events
		bool onUpdate(Event* ev, Unit* unit);
		bool onDelete(Event* ev, Unit* unit);

		bool FillPoseidonMesh(Poseidon::Core* core, Mesh* mesh, Poseidon::Mesh* out);

	protected:
		bool ensureMaterialElement(Material* material);
	};
}

#endif // !RHEA_GPAPHICS_MESHMANAGER_H
