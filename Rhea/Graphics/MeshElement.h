#ifndef RHEA_GRAPHICS_MESH_MGR_H
#define RHEA_GRAPHICS_MESH_MGR_H

#include <vector>
#include "../Element.h"

namespace Rhea
{
	class GraphicsMesh : public Element
	{
	public:
		Poseidon::Mesh* m_mesh;
	};	
}

#endif // !RHEA_GRAPHICS_MESH_MGR_H
