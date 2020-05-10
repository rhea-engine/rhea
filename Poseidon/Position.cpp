#include "Position.h"
#include "Node.h"

namespace Poseidon
{
	Position::Position(Node* owner) :
		m_position(Vector3(0, 0, 0)),
		m_yaw_fixed_axis(Vector3::UNIT_Y),
		m_owner(owner)
	{}

	void Position::setPosition(float x, float y, float z)
	{
		m_position.x = x;
		m_position.y = y;
		m_position.z = z;
		if (m_owner)
			m_owner->needUpdate();
	}

	void Position::setPosition(const Vector3& vec)
	{
		m_position = vec;
		if (m_owner)
			m_owner->needUpdate();
	}

	void Position::setPosition(const Position& position)
	{
		m_position = position.m_position;
		m_rotation = position.m_rotation;
		m_yaw_fixed_axis = position.m_yaw_fixed_axis;
		if (m_owner)
			m_owner->needUpdate();
	}

	void Position::setDirection(float x, float y, float z, const Vector3& localDirectionVector)
	{
		setDirection(Vector3(x, y, z), localDirectionVector);		
	}

	void Position::setDirection(const Vector3& direction, const Vector3& localDirectionVector)
	{
		// The direction we want the local direction point to
		Vector3 forward = direction.normalisedCopy();

		// Calculate target orientation relative to world space
		Quaternion targetOrientation;
		if (true) //(mYawFixed)
		{
			// Calculate the quaternion for rotate local Z to target direction
			Vector3 yawAxis = m_yaw_fixed_axis;

			/*
			if (mInheritOrientation && mParent)
			{
				yawAxis = mParent->_getDerivedOrientation() * yawAxis;
			}
			*/

			Vector3 right = yawAxis.crossProduct(forward);
			right.normalise();
			Vector3 up = forward.crossProduct(right);
			up.normalise();			
			Quaternion unitZToTarget = Quaternion(right, up, forward);

			if (localDirectionVector == Vector3::NEGATIVE_UNIT_Z)
			{
				// Special case for avoid calculate 180 degree turn
				targetOrientation =
					Quaternion(-unitZToTarget.y, -unitZToTarget.z, unitZToTarget.w, unitZToTarget.x);
			}
			else
			{
				// Calculate the quaternion for rotate local direction to target direction
				//Quaternion localToUnitZ = localDirectionVector.getRotationTo(Vector3::UNIT_Z);
				//targetOrientation = unitZToTarget * localToUnitZ;
			}
		}

		setOrientation(targetOrientation);
	}

	void Position::setOrientation(const Quaternion& q)
	{
		m_rotation = q;
		if (m_owner)
			m_owner->needUpdate();
	}

	void Position::lookAt(const Vector3& targetPoint, const Vector3& localDirectionVector)
	{
		// Calculate ourself origin relative to the given transform space
		Vector3 origin;

		// only world space for now
		origin = m_position;
		/*
		switch (relativeTo)
		{
		default:    // Just in case
		case TS_WORLD:
			origin = _getDerivedPosition();
			break;
		case TS_PARENT:
			origin = mPosition;
			break;
		case TS_LOCAL:
			origin = Vector3::ZERO;
			break;
		}
		*/
		setDirection(targetPoint - origin, localDirectionVector);
	}

	void Position::moveRelative(const Vector3& vec)
	{
		// Transform the axes of the relative vector by camera's local axes
		Vector3 trans = m_rotation * vec;

		m_position = m_position + trans;
		if (m_owner)
			m_owner->needUpdate();
	}

	Matrix3x4 Position::getWorldTransform()
	{
		return Matrix3x4(m_position, m_rotation, 1.0f);
	}
}