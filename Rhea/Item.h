#ifndef REHA_ITEM_H
#define REHA_ITEM_H

#include "Resource.h"

namespace Poseidon
{
	class Entity;
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
			m_app(NULL),
			m_entity(NULL),
			m_mesh(NULL),
			Resource(name, "//meworla/item")
		{}

		// setup from strings
		void setup(Application* app, std::unordered_map<std::string, std::string> attributes, std::unordered_map<std::string, std::string> values);

		// load item
		void load(ReadyHandler handler);

		// show in scene
		void show(Poseidon::Core* core);

		// factory function
		static Unit* createType(std::string name, std::string type);
	protected:
		void loadDependencies(ReadyHandler handler);
		Poseidon::Entity* m_entity;
		Application* m_app;
		Mesh* m_mesh;
	};
}
#endif // !REHA_ITEM_H
