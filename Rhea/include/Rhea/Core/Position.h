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
	protected:
		math::Vector3 m_position;
		math::Quaternion m_rotation;
		math::Matrix3x4 m_wtm;

		/// Fixed axis to yaw around
		math::Vector3 m_yaw_fixed_axis;

		bool m_dirty_wtm;
	public:
		Position();
		void setPosition(float x, float y, float z);
		void setPosition(const Vector3& vec);
		void setPosition(const Position& position);
		void setDirection(float x, float y, float z, const Vector3& localDirectionVector = Vector3::NEGATIVE_UNIT_Z);
		void setDirection(const Vector3& vec, const Vector3& localDirectionVector = Vector3::NEGATIVE_UNIT_Z);
		Vector3 moveRelative(const Vector3& vec);
		void setOrientation(const math::Quaternion& q);
		void lookAt(const Vector3& targetPoint, const Vector3& localDirectionVector = Vector3::NEGATIVE_UNIT_Z);
		math::Matrix3x4 getWorldTransform();
		Vector3 get_worldspace() { return m_position; }
		math::Quaternion get_rotation() { return m_rotation; }
	};
}
#endif // !RHEA_POSITION_H
