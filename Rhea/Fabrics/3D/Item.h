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

	/** 
	* Item is the base class for all units that can be placed in the scene with a 3D location.
	* For adding behahviour or visual representation add components as childs
	*/
	class Item : public Unit
	{
		MECLASS(Item, Unit)

	public:
		Item(std::string name) :
			Item(name, "//meworla/item")
		{}
		Item(std::string name, std::string type_name) :
			m_graphics_item(nullptr),
			m_physics_body(nullptr),
			m_turn(0.0f),			
			m_transform(this),
			m_render_component(nullptr),
			Unit(name, type_name)
		{}
		
		// setup from strings
		void setup(Application* app, std::unordered_map<std::string, std::string> attributes, std::unordered_map<std::string, Variant> values);

		// ready event handler
		void ready();

		// get dependent data
		Mesh* getMesh() { if (m_render_component) return m_render_component->getMesh(); else if (getComponent<RenderComponent>()) return getComponent<RenderComponent>()->getMesh(); else return nullptr; }

		std::vector<Export> getReferences();

		// instancing
		virtual Item* instance(std::string name);

		// positioning
		Transform* getTransform() { return &m_transform; }

		// update on frame
		virtual void onFrame(float delta);

		virtual void setPosition(Rhea::Position& position);
		virtual void setPosition(Math::Vector3 position);

		// set direction
		virtual void setLookAt(Math::Vector3 look_at);

		// movement
		virtual void setMovement(Math::Vector3 move);
		virtual void setTurn(float turn);
		Math::Vector3 getMovement() { return m_movement; }
		float getTurn() { return m_turn; }
		virtual void jump() {}

		// graphics
		RenderComponent* getRenderComponent() {	if (m_render_component) return m_render_component; else return getComponent<RenderComponent>(); }

		// visibility
		virtual void hide() { if (m_render_component) m_render_component->hide(); else if (getComponent<RenderComponent>()) getComponent<RenderComponent>()->hide(); }
		virtual void show() { if (m_render_component) m_render_component->show(); else if (getComponent<RenderComponent>()) getComponent<RenderComponent>()->show(); }
		bool isVisible() { if (m_render_component) return m_render_component->isVisible(); else if (getComponent<RenderComponent>()) return getComponent<RenderComponent>()->isVisible(); else return false; }

		// networking
		void setNetworkComponent(std::unique_ptr<NetworkComponent> component) { m_net_component = std::move(component); }
		NetworkComponent* getNetworkComponent() { return m_net_component.get(); }

		// physics
		PhysicsBody* getPhysicsBody();

	protected:
		GraphicsRenderElement* m_graphics_item;
		PhysicsBody* m_physics_body;
		std::unique_ptr<NetworkComponent> m_net_component;
		Math::Vector3 m_movement;
		float m_turn;
		RenderComponent* m_render_component;

		bool getGraphicsItem();

		Transform m_transform;
	};
}
#endif // !REHA_ITEM_H
