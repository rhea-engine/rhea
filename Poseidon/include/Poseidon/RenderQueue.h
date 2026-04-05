#ifndef POSEIDON_RENDERQUEUE_H
#define POSEIDON_RENDERQUEUE_H

#include <vector>
#include <Rhea/Core/Item.h>

using rhea::Item;

namespace poseidon
{
	class RenderQueue
	{
	public:
		void addRenderable(Item* renderable);
		void clear();
		Item* first();
		Item* next();
	protected:
		std::vector<Item*> m_render_items;
		std::vector<Item*>::iterator it;
	};
}

#endif // !POSEIDON_RENDERQUEUE_H
