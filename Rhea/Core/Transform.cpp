#include "Rhea/Core/Transform.h"
#include "Rhea/Core/Item.h"
#include <Psio/Log.h>

namespace rhea
{
	void Transform::update()
	{
		if (m_in_updating) {
			LOG(LOG_ERROR) << "Transform: Circular dependency detected in update of " << m_parent->getPath();
			return;
		}

		// update position
		if (m_parent) {

			m_in_updating = true;

			// TODO: Check if transform component is there
			Item* parent = doCast<Item>(m_parent->getParent());
			if (!parent) {
				m_derived_position.setPosition(m_position);
				m_dirty = false;
				m_in_updating = false;
				return;
			}

			// Update orientation
			const math::Quaternion& parent_rot = parent->getTransform()->get_position()->get_rotation();
			const Position* src = get_position();
			Position* derived = &m_derived_position; // get_derived_position();
			if (m_coord_space == 1)
			{
				// Combine orientation with that of parent
				derived->setOrientation(parent_rot * src->get_rotation());
			}
			else {
				derived->setOrientation(get_position()->get_rotation());
			}

			// Change position vector based on parent's orientation & scale
			math::Vector3 derived_pos = parent_rot * src->get_worldspace();

			// Add altered position vector to parents
			derived_pos += parent->getTransform()->get_derived_position()->get_worldspace();
			derived->setPosition(derived_pos);

			m_dirty = false;
			m_in_updating = false;

			//m_parent->update();
		}
		else {
			// root node 
			m_derived_position.setPosition(m_position);
			m_dirty = false;
		}
	}

	void Transform::createPosition(int position_type)
	{
		m_coord_space = position_type;
		m_dirty = true;
		m_position.setPosition(math::Vector3(0.0f, 0.0f, 0.0f));
		m_derived_position.setPosition(math::Vector3(0.0f, 0.0f, 0.0f));
	}

	void Transform::setPosition(const Position& position)
	{
		m_position.setPosition(position);
		m_dirty = true;
		update();
	}

	void Transform::markDirty()
	{
		m_dirty = true;
		m_position.markDirty();
		m_derived_position.markDirty();
	}

	Transform* Transform::createType(std::string name, std::string type_name)
	{
		return nullptr;
	}

	void Transform::initializeType(simpleparser::Type* type)
	{
	}
}
