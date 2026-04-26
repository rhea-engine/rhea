#include "Rhea/Core/Position.h"
#include <Psio/Log.h>

namespace rhea
{
	Position::Position() :
		m_position(Vector3(0, 0, 0)),
		m_yaw_fixed_axis(Vector3::UNIT_Y),
		m_dirty(true)
	{}

	void Position::setPosition(float x, float y, float z)
	{
		m_position.x = x;
		m_position.y = y;
		m_position.z = z;
		m_dirty = true;
	}

	void Position::setPosition(const Vector3& vec)
	{
		m_position = vec;
		m_dirty = true;		
	}

	void Position::setPosition(const Position& position)
	{
		m_position = position.m_position;
		m_rotation = position.m_rotation;
		m_yaw_fixed_axis = position.m_yaw_fixed_axis;
		m_dirty = true;
	}

	void Position::setDirection(float x, float y, float z, const Vector3& localDirectionVector)
	{
		setDirection(Vector3(x, y, z), localDirectionVector);		
		m_dirty = true;
	}

	void Position::setDirection(const Vector3& direction, const Vector3& localDirectionVector)
	{
		Vector3 forward = direction.normalizedCopy();
		if (forward == Vector3::ZERO)
			return;
		
		m_dirty = true;

		Vector3 yawAxis = m_yaw_fixed_axis;
		Vector3 right = yawAxis.crossProduct(forward);
		
		// DEBUG: Check if cross product is zero (parallel vectors!)
		if (right.length() < 0.0001f) {
			LOG(LOG_ERROR) << "setDirection: forward and yawAxis are parallel! forward=" 
			               << forward.x << "," << forward.y << "," << forward.z;
			// Fallback: use a different up vector
			yawAxis = Vector3::UNIT_X;
			right = yawAxis.crossProduct(forward);
		}
		
		right.normalize();
		Vector3 up = forward.crossProduct(right);
		up.normalize();

		Quaternion targetOrientation;

		if (localDirectionVector == Vector3::NEGATIVE_UNIT_Z)
		{
			targetOrientation = Quaternion(right, up, -forward);
		}
		else if (localDirectionVector == Vector3::UNIT_Z)
		{
			targetOrientation = Quaternion(right, up, forward);
		}
		else
		{
			targetOrientation = Quaternion(right, up, -forward);
		}

		LOG(LOG_DEBUG) << "setDirection result quat: " << targetOrientation.w << "," 
		               << targetOrientation.x << "," << targetOrientation.y << "," << targetOrientation.z;

		setOrientation(targetOrientation);
	}

	void Position::setOrientation(const Quaternion& q)
	{
		m_rotation = q;
		m_dirty = true;
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

	Vector3 Position::moveRelative(const Vector3& vec)
	{
		// Transform the axes of the relative vector by camera's local axes
		Vector3 trans = m_rotation * vec;

		m_position = m_position + trans;
		m_dirty = true;
		
		return m_position;		
	}

	Matrix3x4 Position::getWorldTransform()
	{
		if (m_dirty) {
			m_wtm = Matrix3x4(m_position, m_rotation, 1.0f);
			m_dirty = false;
		}
		return m_wtm;
	}
}