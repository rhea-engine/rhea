/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#include "Rhea/Core/Item.h"
#include "Rhea/Application.h"
#include "Rhea/UnitTree.h"
#include <Rhea/Core/Mesh.h>
#include <Psio/Log.h>
#include <Math/Math.h>
#include <Rhea/Components/MeshComponent.h>
#include <algorithm>

namespace rhea
{
	void Item::setup(Application* app, std::unordered_map<std::string, std::string> attributes, std::unordered_map<std::string, Variant> values)
	{
		Unit::setup(app, attributes, values);
		
		// legacy - set position and rotation from values in itemsetup
		// assume no transform component exists yet, so create one
		if (m_values.find("position") != m_values.end() || m_values.find("rotation") != m_values.end())
		{
			ensureTransform();


			Variant pos_sit = getValue("position");
			if (pos_sit != Variant()) {
				float x = pos_sit[0];
				float y = pos_sit[1];
				float z = 0.0f;
				if (pos_sit.size() > 2) {
					z = pos_sit[2];
				}
				Position position;
				position.setPosition(x, y, z);
				m_transform->setLocalPosition(position);
			}
			if (m_values.find("rotation") != m_values.end())
			{
				Variant rot_sit = getValue("rotation");
				if (rot_sit != Variant() && rot_sit.size() == 3) {
					float x = rot_sit[0];
					float y = rot_sit[1];
					float z = rot_sit[2];
					math::Quaternion rot;
					if (x != 0.0f)
						rot.fromAngleAxis(x * math::degToRad, math::Vector3(1.0f, 0.0f, 0.0f));
					if (y != 0.0f)
						rot.fromAngleAxis(y * math::degToRad, math::Vector3(0.0f, 1.0f, 0.0f));
					if (z != 0.0f)
						rot.fromAngleAxis(z * math::degToRad, math::Vector3(0.0f, 0.0f, 1.0f));
					Position position = m_transform->getLocalPosition();
					position.setOrientation(rot);
					m_transform->setLocalPosition(position);
				}
			}
		}

		if (m_values.find("mesh") != m_values.end()) {
			std::string mesh_id = m_values["mesh"].toString();
			VisualComponent* visual_component = getComponent<VisualComponent>();
			if (!visual_component) {
				visual_component = addComponentUnique<VisualComponent>("visual_component");
			}
			MeshComponent* mesh_component = getComponent<MeshComponent>();
			if (!mesh_component) {
				mesh_component = addComponentUnique<MeshComponent>("mesh_component");
				mesh_component->setOrigin(Component::ComponentOrigin::Configured);
				mesh_component->setAttribute("basepath", m_attributes["basepath"]);

				// set ref only if mesh component was newly created
				mesh_component->setMeshRef(Ref<Mesh>(mesh_id, m_attributes["basepath"]));
			}
		}

	}

	void Item::ensureTransform()
	{
		if (!m_transform)
		{
			m_transform = getComponent<Transform>();
			if (!m_transform)
				m_transform = addComponentUnique<Transform>("transform");
		}
	}

	void Item::ensureVisualComponent()
	{
		if (!getComponent<VisualComponent>()) {
			addComponentUnique<VisualComponent>("visual_component");
		}
	}

	void Item::ready()
	{
		Unit::ready();

		ensureTransform();

		VisualComponent* visual_component = getComponent<VisualComponent>();

		if (getComponent<MeshComponent>()) {
			if (!visual_component) {
				visual_component = addComponentUnique<VisualComponent>("visual_component");
			}
		}

		if (visual_component) {
			if (m_values.find("fabric") != m_values.end())
				visual_component->hide();
			if (m_values.find("hidden") != m_values.end()) {
				if (m_values["hidden"].toBool())
					visual_component->hide();
				else
					visual_component->show();
			}
			if (m_values.find("render_phase") != m_values.end()) {
				std::string phase = m_values["render_phase"].toString();
				if (phase == "opaque")
					visual_component->setRenderPhase(RenderPhase::Opaque);
				else if (phase == "transparent")
					visual_component->setRenderPhase(RenderPhase::Transparent);
				else if (phase == "overlay")
					visual_component->setRenderPhase(RenderPhase::Overlay);
				else if (phase == "background")
					visual_component->setRenderPhase(RenderPhase::Background);
				else
					LOG(LOG_WARNING) << "unknown render_phase value: " << phase;

			}
		}
	}

	void Item::onParented()
	{
		Unit::onParented();
		if (m_transform)
			m_transform->markWorldDirty();
		Event add_event("attached_item", this);
		bubbleEvent("attached_item", &add_event);
	}

	void Item::onRemoved()
	{
		Unit::onRemoved();
		Event add_event("detached_item", this);
		bubbleEvent("detached_item", &add_event);		
	}

	bool Item::canRemoveComponent(const	Component* component) const
	{
		if (component == m_transform) {
			LOG(LOG_ERROR) << "cannot remove transform component from item";
			return false;
		}
		return true;
	}

	void Item::onFrame(float delta)
	{
	}

	void Item::onTransformChanged()
	{
		for (MovementListenerComponent* listener : m_movement_listeners) {
			listener->onTransformChanged();
		}
	}

	void Item::setLocalPosition(rhea::Position& position)
	{		
		ensureTransform();
		// check if position updated
		Position t_position = m_transform->getLocalPosition();
		if (position.nearlyEquals(t_position))
			return;		
		m_transform->setLocalPosition(position);				
	}
	
	void Item::setLocalPosition(math::Vector3 position)
	{
		ensureTransform();
		Position t_position = m_transform->getLocalPosition();
		t_position.setPosition(position);
		m_transform->setLocalPosition(t_position);		
	}

	void Item::setWorldPosition(rhea::Position& position)
	{
		ensureTransform();
		Position t_position = m_transform->getWorldPosition();
		if (position.nearlyEquals(t_position))
			return;
		m_transform->setWorldPosition(position);		
	}

	void Item::setWorldPosition(math::Vector3 position)
	{
		ensureTransform();
		Position t_position = m_transform->getWorldPosition();
		t_position.setPosition(position);
		m_transform->setWorldPosition(t_position);
	}

	void Item::setLookAt(math::Vector3 look_at, const math::Vector3& localDirectionVector)
	{
		ensureTransform();
		Position t_position = m_transform->getWorldPosition();
		t_position.lookAt(look_at, localDirectionVector);
		m_transform->setWorldPosition(t_position);
	}
		
	void Item::setMovement(math::Vector3 move)
	{
		m_movement = move;
		for (auto* l : m_movement_listeners) {
			l->onMovementChanged(move);
		}		
	}	

	void Item::setWorldMovement(
		const math::Vector3& world_movement)
	{
		const math::Quaternion orientation =
			getTransform()->getWorldPosition().getRotation();

		const math::Quaternion inverse_orientation(
			orientation.w,
			-orientation.x,
			-orientation.y,
			-orientation.z
		);

		setMovement(inverse_orientation * world_movement);
	}

	void Item::setTurn(float turn)
	{
		m_turn = turn;
		for (auto* l : m_movement_listeners) {
			l->onTurnChanged(turn);
		}
	}	

	void Item::addComponent(Component* c)
	{
		Unit::addComponent(c);

		if (auto* transform = doCast<Transform>(c))
			m_transform = transform;

		if (auto* l = doCast<MovementListenerComponent>(c))
			m_movement_listeners.push_back(l);
	}

	void Item::removeComponent(Component* c)
	{
		if (auto* l = doCast<MovementListenerComponent>(c)) {
			auto it = std::remove(m_movement_listeners.begin(), m_movement_listeners.end(), l);
			m_movement_listeners.erase(it, m_movement_listeners.end());
		}
		Unit::removeComponent(c);
	}
}