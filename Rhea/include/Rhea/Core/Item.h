/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#ifndef REHA_ITEM_H
#define REHA_ITEM_H

#include <memory>

#include <Rhea/Core/ActiveUnit.h>

#include <Rhea/Core/Position.h>
#include <Math/Vector3.h>
#include <Rhea/Core/Transform.h>
#include <Rhea/Components/VisualComponent.h>
#include <Rhea/Components/MovementListenerComponent.h>

#include <Rhea/Ref.h>

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
	class Item : public ActiveUnit
	{
		RECLASS(Item, ActiveUnit, "//meworla/item");		
	public:
		Item(std::string name, std::string type_name) :
			m_physics_body(nullptr),
			m_turn(0.0f),			
			m_transform(nullptr),
			m_is_renderable(true),
			ActiveUnit(name, type_name)
		{}
		
		// setup from strings
		void setup(Application* app, std::unordered_map<std::string, std::string> attributes, std::unordered_map<std::string, Variant> values);

		// ready event handler
		void ready();

		// handle adding to tree
		void onParented() override;

		// handle removing from tree
		void onRemoved() override;		

		// dont remove transform component
		bool canRemoveComponent(const Component* component) const override;

		std::vector<Export> getReferences();

		// positioning
		Transform* getTransform() { ensureTransform(); return m_transform; }

		// update on frame
		void onFrame(float delta) override;

		void onTransformChanged();

		virtual void setLocalPosition(rhea::Position& position);
		virtual void setLocalPosition(math::Vector3 position);
		virtual void setWorldPosition(rhea::Position& position);
		virtual void setWorldPosition(math::Vector3 position);

		// set direction
		virtual void setLookAt(math::Vector3 look_at, const math::Vector3& localDirectionVector);

		// movement
		// Movement relative to the item's current orientation.
		virtual void setMovement(math::Vector3 move);
		// Movement expressed in world coordinates.
		virtual void setWorldMovement(const math::Vector3& move);
		virtual void setTurn(float turn);
		math::Vector3 getMovement() { return m_movement; }
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
		void ensureTransform();
		PhysicsBody* m_physics_body; // cached - not owned
		math::Vector3 m_movement;
		float m_turn;				
		std::vector<MovementListenerComponent*> m_movement_listeners;
		bool m_is_renderable;

		Transform* m_transform;
	};
}
#endif // !REHA_ITEM_H
