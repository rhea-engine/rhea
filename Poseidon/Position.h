#ifndef POSEIDON_POSITION_H
#define POSEIDON_POSITION_H

#include "Math/Vector3.h"
#include "Math/Quaternion.h"
#include "Math/Matrix3x4.h"
#include "Math/Matrix4.h"

#include <DirectXMath.h>
using DirectX::XMMATRIX;
using DirectX::XMVECTOR;

namespace Poseidon
{
	class Node;

	class Position
	{
	protected:
		Vector3 m_position;
		Quaternion m_rotation;

		Node* m_owner;

		/// Fixed axis to yaw around
		Vector3 m_yaw_fixed_axis;
	public:
		Position(Node* owner);
		void setPosition(float x, float y, float z);
		void setPosition(const Vector3& vec);
		void setPosition(const Position& position);
		void setDirection(float x, float y, float z, const Vector3& localDirectionVector = Vector3::NEGATIVE_UNIT_Z);
		void setDirection(const Vector3& vec, const Vector3& localDirectionVector = Vector3::NEGATIVE_UNIT_Z);
		void moveRelative(const Vector3& vec);
		void setOrientation(const Quaternion& q);
		void lookAt(const Vector3& targetPoint, const Vector3& localDirectionVector = Vector3::NEGATIVE_UNIT_Z);
		Matrix3x4 getWorldTransform();
		Vector3 get_worldspace() { return m_position; }
		Quaternion get_rotation() { return m_rotation; }
	};
}
#endif // !POSEIDON_POSITION_H
