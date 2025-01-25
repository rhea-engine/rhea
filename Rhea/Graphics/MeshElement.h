#ifndef RHEA_GRAPHICS_MESH_MGR_H
#define RHEA_GRAPHICS_MESH_MGR_H

#include <vector>
#include <Poseidon/Mesh.h>
#include "GraphicsElement.h"

namespace Rhea
{
	class GraphicsMesh : public GraphicsElement
	{
	public:
		Poseidon::Mesh* m_mesh = nullptr;
	};	
}

#endif // !RHEA_GRAPHICS_MESH_MGR_H
