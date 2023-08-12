#ifndef REHA_ITEM_H
#define REHA_ITEM_H

#include <memory>

#include <Rhea/Unit.h>

#include "Rhea/Fabrics/Core/Position.h"
#include "Math/Vector3.h"
#include "../Core/Transform.h"
#include "../Network/NetworkComponent.h"
#include <Rhea/Components/RenderComponent.h>

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

	class Item : public Unit
	{
		MECLASS(Item, Unit)

	public:
		Item(std::string name) :
			Item(name, "//meworla/item")
		{}
		Item(std::string name, std::string type_name) :
			m_mesh(nullptr),
			m_graphics_item(nullptr),
			m_physics_body(nullptr),
			m_turn(0.0f),
			m_material(nullptr),
			m_transform(this),
			m_render_component(nullptr),
			Unit(name, type_name)
		{}
		virtual ~Item() = default;

		// setup from strings
		void setup(Application* app, std::unordered_map<std::string, std::string> attributes, std::unordered_map<std::string, Variant> values);

		// ready event handler
		void ready();

		// get dependent data
		Mesh* getMesh() { return m_mesh; }

		// material
		Material* getMaterial() { return m_material; }

		std::vector<Export> getReferences();

		// instancing
		virtual Item* instance(std::string name);

		// positionig
		Transform* getTransform() { return &m_transform; }

		// update on frame
		virtual void onFrame(float delta);

		virtual void setPosition(Rhea::Position& position);
		virtual void setPosition(Math::Vector3 position);

		// set direction
		virtual void setLookAt(Math::Vector3 look_at);

		// movement
		void setMovement(Math::Vector3 move);
		void setTurn(float turn);
		Math::Vector3 getMovement() { return m_movement; }
		float getTurn() { return m_turn; }
		virtual void jump() {}

		// graphics
		RenderComponent* getGraphicsComponent() { return m_render_component.get();}

		// networking
		void setNetworkComponent(std::unique_ptr<NetworkComponent> component) { m_net_component = std::move(component); }
		NetworkComponent* getNetworkComponent() { return m_net_component.get(); }

		// physics
		PhysicsBody* getPhysicsBody();

	protected:
		Mesh* m_mesh;
		GraphicsRenderElement* m_graphics_item;
		std::unique_ptr<RenderComponent> m_render_component;
		PhysicsBody* m_physics_body;
		std::unique_ptr<NetworkComponent> m_net_component;
		Math::Vector3 m_movement;
		float m_turn;
		Material* m_material;

		bool getGraphicsItem();

		Transform m_transform;
	};
}
#endif // !REHA_ITEM_H
