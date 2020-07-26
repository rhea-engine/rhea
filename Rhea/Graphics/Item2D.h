#ifndef RHEA_GRAPHICS_Item2D_H
#define RHEA_GRAPHICS_Item2D_H

#include <Poseidon/Core.h>
#include <Poseidon/Node.h>
#include <Poseidon/RenderContext.h>

#include "../Element.h"

namespace Rhea
{
	class Unit;
	class Item2D;

	class GraphicsItem2D : public Element
	{
	public:
		GraphicsItem2D(Poseidon::Core* core) :
			m_core(core),
			m_entity(nullptr),
			m_render_context(nullptr)
		{}
		~GraphicsItem2D();

		// create a Item2D from buffer
		void createItem2D(Item2D* item);

		static Element* createElement(Poseidon::Core* core, Unit* unit);

	protected:
		Poseidon::Core* m_core;
		Poseidon::Node* m_entity;
		Poseidon::RenderContext* m_render_context;
	};
}

#endif // !RHEA_GRAPHICS_ITEM_H
