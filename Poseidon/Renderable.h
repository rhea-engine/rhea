#ifndef POSEIDON_RENDERABLE_H
#define POSEIDON_RENDERABLE_H

#include "Node.h"

namespace Hippios
{
	class VertexBuffer;
	class IndexBuffer;
}

namespace Poseidon
{
	class RenderQueue;
	class Camera;
	class Texture;

	class Renderable : public Node
	{
	public:
		virtual void cullPhase(RenderQueue* queue, Camera* camera) = 0;
		virtual void getRenderInput(Hippios::VertexBuffer** pvb, Hippios::IndexBuffer** pib) = 0;
		virtual Texture* getMaterial() = 0;
	};
}

#endif // !POSEIDON_RENDERABLE_H
