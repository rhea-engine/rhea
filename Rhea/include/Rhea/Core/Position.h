/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#ifndef RHEA_POSITION_H
#define RHEA_POSITION_H

#include "Math/Vector3.h"
#include "Math/Quaternion.h"
#include "Math/Matrix3x4.h"
#include "Math/Matrix4.h"

using math::Vector3;
using math::Quaternion;
using math::Matrix3x4;

namespace rhea
{
	class Position
	{		
	public:
		Position();
		Position(const Vector3& position, const Quaternion& rotation);
		Position(const Position& other);
		Position(const Vector3& position);
		Position(const Quaternion& rotation);
		Position(const Matrix3x4& matrix);
		void setPosition(float x, float y, float z);
		void setPosition(const Vector3& vec);
		void setTranslation(const Vector3& vec);		
		void setDirection(float x, float y, float z, const Vector3& localDirectionVector = Vector3::NEGATIVE_UNIT_Z);
		void setDirection(const Vector3& vec, const Vector3& localDirectionVector = Vector3::NEGATIVE_UNIT_Z);
		Vector3 moveRelative(const Vector3& vec);
		// legacy - use setRotation
		void setOrientation(const math::Quaternion& q) { setRotation(q); }
		void setRotation(const math::Quaternion& q);
		void lookAt(const Vector3& targetPoint, const Vector3& localDirectionVector = Vector3::NEGATIVE_UNIT_Z);
				
		Vector3 getTranslation() const { return m_position; }				
		math::Quaternion getRotation() const { return m_rotation; }

		bool nearlyEquals(const Position& other, float epsilon = (float)1e-06) const {
			return m_position.equals(other.m_position, epsilon) && m_rotation.equals(other.m_rotation, epsilon);
		}

	protected:
		math::Vector3 m_position;
		math::Quaternion m_rotation;

		/// Fixed axis to yaw around
		math::Vector3 m_yaw_fixed_axis;
	};
}
#endif // !RHEA_POSITION_H
