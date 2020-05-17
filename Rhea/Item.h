#ifndef REHA_ITEM_H
#define REHA_ITEM_H

#include "Resource.h"

#include "Poseidon/Position.h"

namespace Poseidon
{
	class Node;
}

namespace Rhea
{
	class Application;
	class Mesh;
	class GraphicsItem;

	class Item : public Resource
	{
	public:
		Item(std::string name) :
			m_mesh(nullptr),
			m_graphics_item(nullptr),
			Resource(name, "//meworla/item")
		{}
		Item(std::string name, std::string type_name) :
			m_mesh(nullptr),
			m_graphics_item(nullptr),
			Resource(name, type_name)
		{}

		// setup from strings
		void setup(Application* app, std::unordered_map<std::string, std::string> attributes, std::unordered_map<std::string, Variant> values);

		// load item
		void load(ReadyHandler handler);

		// get dependent data
		Mesh* getMesh() { return m_mesh; }

		// factory function
		static Unit* createType(std::string name, std::string type);

		// positionig
		virtual void setPosition(Poseidon::Position& position);

		// set direction
		virtual void setLookAt(Poseidon::Vector3 look_at);

		// an item that follows this item
		void addFollower(Item* item);

		// get the unerlying entity
		virtual Poseidon::Node* getNode();

	protected:
		Mesh* m_mesh;
		GraphicsItem* m_graphics_item;

		void loadDependencies(ReadyHandler handler);
		bool getGraphicsItem();
	};
}
#endif // !REHA_ITEM_H
