#ifndef POSEIDON_RENDERABLE_H
#define POSEIDON_RENDERABLE_H

#include <vector>

#include "Node.h"
#include <Rhea/Core/Transform.h>

namespace Hippios
{
	class VertexBuffer;
	class IndexBuffer;
}

namespace Poseidon
{
	class RenderQueue;
	class Camera;
	class Material;

	class Renderable : public Node
	{
	public:
		virtual void cullPhase(RenderQueue* queue, Camera* camera) = 0;
		virtual void getRenderInput(size_t mesh_idx, Hippios::VertexBuffer** pvb, Hippios::IndexBuffer** pib, size_t& num_indices) = 0;
		virtual Material* getMaterial() = 0;
	};
}

#endif // !POSEIDON_RENDERABLE_H
