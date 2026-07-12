/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#ifndef RHEA_FABRICS_TRANSFORM
#define RHEA_FABRICS_TRANSFORM

#include <string>

#include <Rhea/Component.h>
#include "Rhea/Core/Position.h"

namespace rhea
{
	class Item;

	class Transform : public Component
	{
		RECLASS(Transform, Component, "//meworla/transform");
		REPROPERTY(math::Vector3, "position", Label("Position"), Edit, Read);
		REPROPERTY(math::Vector3, "rotation", Label("Rotation"), Edit, Read);
		REPROPERTY(math::Vector3, "scale", Label("Scale"), Edit, Read);
	public:
		Transform(std::string name, std::string type_name) :
			m_position(),
			m_derived_position(),
			m_coord_space(1),
			m_dirty(true),
			m_in_updating(false),
			m_scale(1.0f, 1.0f, 1.0f),
			Component(type_name)
		{}	

		// get position
		const Position* get_position() { if (m_dirty) update(); return &m_position; }
		const Position* get_derived_position() { if (m_dirty) update(); return &m_derived_position; }

		// update the position
		void update();
		void setPosition(const Position& position);

		// create a position of the given type
		void createPosition(int position_type);

		// set the coord space 0: world, 1: parent
		void setCoordSpace(int space) { m_coord_space = space; }

		void setScale(const math::Vector3& scale)
		{
			m_scale = scale;
			markWorldDirty();			
		}
		math::Vector3 getScale() const { return m_scale; }

		// mark dirty
		void markDirty();
		void markWorldDirty();

	protected:		
		Position m_position;
		Position m_derived_position;
		int m_coord_space;
		math::Vector3 m_scale;
		
		bool m_in_updating;
		bool m_dirty;
	};
}

#endif // !RHEA_FABRICS_TRANSFORM
