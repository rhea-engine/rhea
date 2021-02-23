#ifndef REHA_ITEM_H
#define REHA_ITEM_H

#include "Resource.h"

#include "Poseidon/Position.h"
#include "Poseidon/Math/Vector3.h"

namespace Poseidon
{
	class Node;
}

namespace Rhea
{
	class Application;
	class Mesh;
	class GraphicsRenderElement;
	class PhysicsBody;
	class Material;

	class Item : public Resource
	{
	public:
		Item(std::string name) :
			m_mesh(nullptr),
			m_graphics_item(nullptr),
			m_physics_body(nullptr),
			m_turn(0.0f),
			m_material(nullptr),
			Resource(name, "//meworla/item")
		{}
		Item(std::string name, std::string type_name) :
			m_mesh(nullptr),
			m_graphics_item(nullptr),
			m_physics_body(nullptr),
			m_turn(0.0f),
			m_material(nullptr),
			Resource(name, type_name)
		{}

		// setup from strings
		void setup(Application* app, std::unordered_map<std::string, std::string> attributes, std::unordered_map<std::string, Variant> values);

		// ready event handler
		void ready();

		// get dependent data
		Mesh* getMesh() { return m_mesh; }

		// material
		Material* getMaterial() { return m_material; }

		// factory function
		static Unit* createType(std::string name, std::string type);

		// positionig
		virtual void setPosition(Poseidon::Position& position);
		virtual void setPosition(Poseidon::Vector3 position);

		// set direction
		virtual void setLookAt(Poseidon::Vector3 look_at);

		// movement
		void setMovement(Poseidon::Vector3 move);
		void setTurn(float turn);
		Poseidon::Vector3 getMovement() { return m_movement; }
		float getTurn() { return m_turn; }
		virtual void jump() {}

		// an item that follows this item
		void addFollower(Item* item);

		// get the unerlying entity
		virtual Poseidon::Node* getNode();

		PhysicsBody* getPhysicsBody();

	protected:
		Mesh* m_mesh;
		GraphicsRenderElement* m_graphics_item;
		PhysicsBody* m_physics_body;
		Poseidon::Vector3 m_movement;
		float m_turn;
		Material* m_material;

		bool getGraphicsItem();
	};
}
#endif // !REHA_ITEM_H
