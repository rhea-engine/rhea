#ifndef RHEA_GRAPHICS_ITEM_H
#define RHEA_GRAPHICS_ITEM_H

#include <Poseidon/Core.h>
#include <Poseidon/Node.h>

#include "../Element.h"

namespace Rhea
{
	class Unit;
	class Item;

	class GraphicsItem : public Element
	{
	public:
		GraphicsItem(Poseidon::Core* core) :
			m_core(core),
			m_entity(nullptr)
		{}
		~GraphicsItem();

		// create a texture from buffer
		void createItem(Item* item);

		// positioning		
		void setPosition(Poseidon::Position& position);
		void setPosition(Poseidon::Vector3 position);

		// set direction
		void lookAt(Poseidon::Vector3 look_at);

		// access entity
		Poseidon::Node* getEntity() { return m_entity; }

		static Element* createElement(Poseidon::Core* core, Unit* unit);

	protected:
		Poseidon::Core* m_core;
		Poseidon::Node* m_entity;		
	};
}

#endif // !RHEA_GRAPHICS_ITEM_H
