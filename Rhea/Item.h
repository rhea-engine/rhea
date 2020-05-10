#ifndef REHA_ITEM_H
#define REHA_ITEM_H

#include "Resource.h"

#include "Poseidon/Position.h"

namespace Poseidon
{
	class Node;
	class Core;
}

namespace Rhea
{
	class Application;
	class Mesh;

	class Item : public Resource
	{
	public:
		Item(std::string name) :
			m_entity(NULL),
			m_mesh(NULL),
			Resource(name, "//meworla/item")
		{}
		Item(std::string name, std::string type_name) :
			m_entity(NULL),
			m_mesh(NULL),
			Resource(name, type_name)
		{}

		// setup from strings
		void setup(Application* app, std::unordered_map<std::string, std::string> attributes, std::unordered_map<std::string, Variant> values);

		// load item
		void load(ReadyHandler handler);

		// show in scene
		virtual void show(Poseidon::Core* core);

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
		void loadDependencies(ReadyHandler handler);
		Poseidon::Node* m_entity;
		Mesh* m_mesh;
	};
}
#endif // !REHA_ITEM_H
