#ifndef REHA_ITEM_H
#define REHA_ITEM_H

#include <memory>

#include <Rhea/Unit.h>

#include <Rhea/Core/Position.h>
#include <Math/Vector3.h>
#include <Rhea/Core/Transform.h>
#include <Rhea/Components/VisualComponent.h>
#include <Rhea/Components/MovementListenerComponent.h>

namespace rhea
{
	class Application;
	class Mesh;
	class GraphicsRenderElement;
	class PhysicsBody;
	class Material;

	/** 
	* Item is the base class for all units that can be placed in the scene with a location.
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
			m_physics_body(nullptr),
			m_turn(0.0f),			
			m_transform(this),
			m_is_renderable(true),
			Unit(name, type_name)
		{}
		
		// setup from strings
		void setup(Application* app, std::unordered_map<std::string, std::string> attributes, std::unordered_map<std::string, Variant> values);

		// ready event handler
		void ready();

		// get dependent data
		//Mesh* getMesh() { if (getComponent<RenderComponent>()) return getComponent<RenderComponent>()->getMesh(); else return nullptr; }

		std::vector<Export> getReferences();

		// instancing
		virtual Item* instance(std::string name);

		// positioning
		Transform* getTransform() { return &m_transform; }

		// update on frame
		virtual void onFrame(float delta);

		virtual void setPosition(rhea::Position& position);
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
		//RenderComponent* getRenderComponent() {	return getComponent<RenderComponent>(); }

		// visibility
		virtual void hide() { if (getComponent<VisualComponent>()) getComponent<VisualComponent>()->hide(); }
		virtual void show() { if (getComponent<VisualComponent>()) getComponent<VisualComponent>()->show(); }
		bool isVisible() { if (getComponent<VisualComponent>()) return getComponent<VisualComponent>()->isVisible(); else return false; }

		// physics
		PhysicsBody* getPhysicsBody() const { return m_physics_body; }
		void setPhysicsBody(PhysicsBody* body) { m_physics_body = body; }

		void addComponent(Component* component) override;
		virtual void removeComponent(Component* component) override;

		bool isRenderable() const { return m_is_renderable; }

	protected:
		PhysicsBody* m_physics_body; // cached - not owned
		Math::Vector3 m_movement;
		float m_turn;				
		std::vector<MovementListenerComponent*> m_movement_listeners;
		bool m_is_renderable;

		Transform m_transform;
	};
}
#endif // !REHA_ITEM_H
