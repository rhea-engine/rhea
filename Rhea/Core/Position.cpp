/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#include "Rhea/Core/Position.h"

namespace rhea
{
	Position::Position() :
		m_position(Vector3::ZERO),
		m_yaw_fixed_axis(Vector3::UNIT_Y)
	{}

	Position::Position(const Vector3& position, const Quaternion& rotation)
	    : m_position(position)
	    , m_rotation(rotation)
	    , m_yaw_fixed_axis(Vector3::UNIT_Y)
	{
	}

	Position::Position(const Position& other)
	    : m_position(other.m_position)
	    , m_rotation(other.m_rotation)
	    , m_yaw_fixed_axis(other.m_yaw_fixed_axis)
	{
	}

	Position::Position(const Vector3& position)
	    : m_position(position)
	    , m_rotation(Quaternion::IDENTITY)
	    , m_yaw_fixed_axis(Vector3::UNIT_Y)
	{
	}

	Position::Position(const Quaternion& rotation)
	    : m_position(Vector3::ZERO)
	    , m_rotation(rotation)
	    , m_yaw_fixed_axis(Vector3::UNIT_Y)
	{
	}

	Position::Position(const Matrix3x4& matrix)
	    : m_position(matrix.Translation())
	    , m_rotation(Quaternion::IDENTITY)
	    , m_yaw_fixed_axis(Vector3::UNIT_Y)
	{
	    m_rotation.fromRotationMatrix(matrix.linear());
	}

	void Position::setPosition(float x, float y, float z)
	{
	    m_position.x = x;
	    m_position.y = y;
	    m_position.z = z;
	}

	void Position::setPosition(const Vector3& vec)
	{
	    m_position = vec;
	}

	void Position::setTranslation(const Vector3& vec)
	{
	    m_position = vec;
	}

	void Position::setDirection(float x, float y, float z, const Vector3& localDirectionVector)
	{
	    setDirection(Vector3(x, y, z), localDirectionVector);
	}

	void Position::setDirection(const Vector3& vec, const Vector3& localDirectionVector)
	{
	    Vector3 targetDir = vec.normalizedCopy();
	    Vector3 currentDir = m_rotation * localDirectionVector;
	    
	    Quaternion rotQuat = currentDir.getRotationTo(targetDir);
	    m_rotation = rotQuat * m_rotation;
	}

	Vector3 Position::moveRelative(const Vector3& vec)
	{
	    Vector3 trans = m_rotation * vec;
	    m_position = m_position + trans;
	    return trans;
	}

	void Position::setRotation(const math::Quaternion& q)
	{
	    m_rotation = q;
	}

	void Position::lookAt(const Vector3& targetPoint, const Vector3& localDirectionVector)
	{
	    Vector3 direction = targetPoint - m_position;
	    setDirection(direction, localDirectionVector);
	}
}