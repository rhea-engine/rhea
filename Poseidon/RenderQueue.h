#ifndef POSEIDON_RENDERQUEUE_H
#define POSEIDON_RENDERQUEUE_H

#include <vector>

namespace Poseidon
{
	class Renderable;

	class RenderQueue
	{
	public:
		void addRenderable(Renderable* renderable);
		void clear();
		Renderable* first();
		Renderable* next();
	protected:
		std::vector<Renderable*> m_render_items;
		std::vector<Renderable*>::iterator it;
	};
}

#endif // !POSEIDON_RENDERQUEUE_H
