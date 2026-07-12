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
	void Transform::updateWorldTransform()
	{
		if (!m_dirty)
			return;

		if (m_in_updating) {
			LOG(LOG_ERROR) << "Transform: Circular dependency detected in update of " << m_parent->getPath();
			return;
		}

		math::Matrix3x4 localMatrix(
			m_local_position.getTranslation(),
			m_local_position.getRotation(),
			1.0
		);

		Item* owner = doCast<Item>(getParent());
		Item* parentItem = owner ? doCast<Item>(owner->getParent()) : nullptr;

		if (parentItem) {
			Transform* parentTransform = parentItem->getTransform();

			const math::Matrix3x4& parentMatrix = parentTransform->worldMatrix();

			m_world_matrix = parentMatrix * localMatrix;

			m_world_position.setTranslation(m_world_matrix.Translation());

			m_world_position.setOrientation(parentTransform->worldPosition().getRotation() * m_local_position.getRotation());

			m_world_scale =	parentTransform->worldScale() * m_local_scale;
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

	void Transform::setPosition(const Position& position)
	{
		m_local_position = position;
		markWorldDirty();		
	}

	void Transform::setScale(const math::Vector3& scale)
	{
		if (scale.equals(m_local_scale))
			return;

		m_local_scale = scale;
		markWorldDirty();
	}

	void Transform::markWorldDirty()
	{
		if (m_dirty)
			return;
		m_dirty = true;
		m_parent->children([](std::string&, Component* component) {
			if (auto* child = doCast<Item>(component))
				child->getTransform()->markWorldDirty();
			return true;
			});
	}
}
