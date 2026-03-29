#ifndef RHEA_GPAPHICS_MESHMANAGER_H
#define RHEA_GPAPHICS_MESHMANAGER_H

#include <Rhea/Graphics/GraphicsManager.h>

#include <Poseidon/Core.h>
#include <Poseidon/Mesh.h>
#include <Rhea/Core/Mesh.h>

#include <unordered_set>

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

		// Removes a mesh from the negative cache so it can be retried
		void clearFailure(const std::string& key) { m_failed_meshes.erase(key); }

	protected:
		bool ensureMaterialElement(Material* material);

		// Negative cache: stores paths of meshes whose build has failed
		std::unordered_set<std::string> m_failed_meshes;
	};
}

#endif // !RHEA_GPAPHICS_MESHMANAGER_H
