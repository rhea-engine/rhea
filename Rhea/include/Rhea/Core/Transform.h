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
			m_local_position(),
			m_world_position(),
			m_coord_space(1),
			m_dirty(true),
			m_in_updating(false),
			m_local_scale(1.0f, 1.0f, 1.0f),
			Component(name, type_name)
		{}	
		void setup(Application* app, std::unordered_map<std::string, std::string> attributes, std::unordered_map<std::string, Variant> values) override;

		void onParented() override;

		// get position
		const Position* get_position() { return &m_local_position; }
		const Position* get_derived_position() { if (m_dirty) updateWorldTransform(); return &m_world_position; }

		const Position& getWorldPosition()
		{
			updateWorldTransform();
			return m_world_position;
		}

		const math::Vector3& getWorldScale()
		{
			updateWorldTransform();
			return m_world_scale;
		}

		const math::Matrix3x4& getWorldMatrix()
		{
			updateWorldTransform();
			return m_world_matrix;
		}

		const Position& getLocalPosition() const
		{
			return m_local_position;
		}

		math::Vector3 getForward() const
		{
			return math::Vector3(
				m_world_matrix[2][0],  // Z.x
				m_world_matrix[2][1],  // Z.y
				m_world_matrix[2][2]   // Z.z
			).normalizedCopy();
		}

		math::Vector3 getRight() const
		{
			return math::Vector3(
				m_world_matrix[0][0],  // X.x
				m_world_matrix[0][1],  // X.y
				m_world_matrix[0][2]   // X.z
			).normalizedCopy();
		}

		// update the position
		void updateWorldTransform();
		void setPosition(const Position& position);

		// create a position of the given type
		void createPosition(int position_type);

		// set the coord space 0: world, 1: parent
		void setCoordSpace(int space) { m_coord_space = space; }

		void setScale(const math::Vector3& scale);
		math::Vector3 getScale() const { return m_local_scale; }

		// mark dirty
		void markWorldDirty();

	protected:		
		// local state
		Position m_local_position;
		math::Vector3 m_local_scale;

		int m_coord_space;
		
		// cached world state
		Position m_world_position;
		math::Vector3 m_world_scale{ 1.0f, 1.0f, 1.0f };
		math::Matrix3x4 m_world_matrix;

		bool m_in_updating;
		bool m_dirty;
	};
}

#endif // !RHEA_FABRICS_TRANSFORM
