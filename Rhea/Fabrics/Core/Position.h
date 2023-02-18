#ifndef RHEA_POSITION_H
#define RHEA_POSITION_H

#include "Math/Vector3.h"
#include "Math/Quaternion.h"
#include "Math/Matrix3x4.h"
#include "Math/Matrix4.h"

using Math::Vector3;
using Math::Quaternion;
using Math::Matrix3x4;

namespace Rhea
{
	class Position
	{
	protected:
		Math::Vector3 m_position;
		Math::Quaternion m_rotation;
		Math::Matrix3x4 m_wtm;

		/// Fixed axis to yaw around
		Math::Vector3 m_yaw_fixed_axis;

		bool m_dirty_wtm;
	public:
		Position();
		void setPosition(float x, float y, float z);
		void setPosition(const Vector3& vec);
		void setPosition(const Position& position);
		void setDirection(float x, float y, float z, const Vector3& localDirectionVector = Vector3::NEGATIVE_UNIT_Z);
		void setDirection(const Vector3& vec, const Vector3& localDirectionVector = Vector3::NEGATIVE_UNIT_Z);
		Vector3 moveRelative(const Vector3& vec);
		void setOrientation(const Math::Quaternion& q);
		void lookAt(const Vector3& targetPoint, const Vector3& localDirectionVector = Vector3::NEGATIVE_UNIT_Z);
		Math::Matrix3x4 getWorldTransform();
		Vector3 get_worldspace() { return m_position; }
		Math::Quaternion get_rotation() { return m_rotation; }
	};
}
#endif // !RHEA_POSITION_H
