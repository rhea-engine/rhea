/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#include "Rhea/Core/Transform.h"
#include "Rhea/Core/Item.h"
#include <Psio/Log.h>

namespace rhea
{
	void Transform::onParented()
	{
		if (!m_parent->getComponent<Transform>()) {
			m_parent->addComponent(this);
		}

		markWorldDirty();
	}

	void Transform::setup(Application* app, std::unordered_map<std::string, std::string> attributes, std::unordered_map<std::string, Variant> values)
	{
		Component::setup(app, attributes, values);
		if (values.find("position") != values.end()) {
			Variant pos = values["position"];
			m_local_position.setPosition(pos[0], pos[1], pos[2]);
		}
		if (values.find("rotation") != values.end()) {
			Variant rot = values["rotation"];
			m_local_position.setRotation(math::Quaternion(rot[0], rot[1], rot[2], rot[3]));
		}
		if (values.find("scale") != values.end()) {
			Variant scale = values["scale"];
			m_local_scale = math::Vector3(scale[0], scale[1], scale[2]);
		}
	}

	void Transform::updateWorldTransform()
	{
		if (!m_dirty)
			return;

		if (m_in_updating) {
			LOG(LOG_ERROR) << "Transform: Circular dependency detected in update of " << m_parent->getPath();
			return;
		}

		m_in_updating = true;

		math::Matrix3x4 localMatrix(
			m_local_position.getTranslation(),
			m_local_position.getRotation(),
			m_local_scale
		);

		Item* owner = doCast<Item>(getParent());
		Item* parentItem = owner ? doCast<Item>(owner->getParent()) : nullptr;

		if (parentItem) {
			Transform* parentTransform = parentItem->getTransform();

			const math::Matrix3x4& parentMatrix = parentTransform->getWorldMatrix();

			m_world_matrix = parentMatrix * localMatrix;

			m_world_position.setPosition(m_world_matrix.Translation());

			m_world_position.setOrientation(parentTransform->getWorldPosition().getRotation() * m_local_position.getRotation());

			m_world_scale =	parentTransform->getWorldScale() * m_local_scale;
		}
		else {
			m_world_matrix = localMatrix;
			m_world_position = m_local_position;
			m_world_scale = m_local_scale;
		}

		m_dirty = false;
		m_in_updating = false;		
	}

	void Transform::createPosition(int position_type)
	{
		m_coord_space = position_type;
		markWorldDirty();
		m_local_position.setPosition(math::Vector3(0.0f, 0.0f, 0.0f));
		m_world_position.setPosition(math::Vector3(0.0f, 0.0f, 0.0f));
	}

	void Transform::setLocalPosition(const Position& position)
	{
		m_local_position = position;
		markWorldDirty();
		Item* owner = doCast<Item>(getParent());
		if (owner) {
			owner->onTransformChanged();
		}
	}

	void Transform::setWorldPosition(const Position& position)
	{
		Item* owner = doCast<Item>(getParent());
		Item* parentItem = owner ? doCast<Item>(owner->getParent()) : nullptr;

		if (!parentItem) {
			setLocalPosition(position);
			return;
		}

		// Convert world position to local position based on parent's world transform
		Transform* parentTransform = parentItem->getTransform();

		// Get parent's world transform components
		const math::Matrix3x4& parentWorld = parentTransform->getWorldMatrix();
		const math::Quaternion& parentWorldRot = parentTransform->getWorldPosition().getRotation();

		// Extract rotation part and invert it
		math::Matrix3 rotPart = parentWorld.linear();
		math::Matrix3 rotInverse = rotPart.Inverse();

		// Calculate relative translation: (worldPos - parentWorldPos) * R^-1
		math::Vector3 relativeTranslation = position.getTranslation() - parentWorld.Translation();
		math::Vector3 localTranslation = rotInverse * relativeTranslation;

		// Calculate local rotation: R_parent^-1 * R_world (conjugate for unit quaternions)
		math::Quaternion parentRotInverse(parentWorldRot.w, -parentWorldRot.x, -parentWorldRot.y, -parentWorldRot.z);
		math::Quaternion localRotation = parentRotInverse * position.getRotation();

		// Set local position
		m_local_position.setPosition(localTranslation);
		m_local_position.setRotation(localRotation);

		m_world_position = position;
		markWorldDirty();
		if (owner) {
			owner->onTransformChanged();
		}
	}

	void Transform::setScale(const math::Vector3& scale)
	{
		if (scale.equals(m_local_scale))
			return;

		m_local_scale = scale;
		markWorldDirty();
		Item* owner = doCast<Item>(getParent());
		if (owner) {
			owner->onTransformChanged();
		}
	}

	void Transform::markWorldDirty()
	{
		if (m_dirty)
			return;
		m_dirty = true;
		m_parent->children([](std::string&, Component* component) {
			if (auto* child = doCast<Item>(component)) {
				child->getTransform()->markWorldDirty();
				child->onTransformChanged();
			}
			return true;
			});
	}
}
