#ifndef RHEA_GRAPHICS_MESH
#define RHEA_GRAPHICS_MESH

#include <Poseidon/Core.h>
#include <Poseidon/Mesh.h>

#include "../Element.h"

namespace Rhea
{
	class Mesh;
	class Unit;

	class GraphicsMesh : public Element {
	public:
		GraphicsMesh(Poseidon::Core* core) :
			m_core(core),
			m_mesh(nullptr)
		{}

		void createMesh(Mesh* mesh);

		// return the mesh
		Poseidon::Mesh* getMesh() { return m_mesh; }

		static Element * createElement(Poseidon::Core* core, Unit* unit);

	protected:
		Poseidon::Core* m_core;
		Poseidon::Mesh* m_mesh;
	};
}
#endif // !RHEA_GRAPHICS_MESH
