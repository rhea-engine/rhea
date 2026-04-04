// Interpolation.h

#pragma once

#include <Math/Vector3.h>
#include <Math/Quaternion.h>

namespace math
{
	template<typename T>
	T lerp(const T& a, const T& b, float t)
	{
		return a + t * (b - a);
	}

	math::Quaternion slerp(const math::Quaternion& q1, const math::Quaternion& q2, float t);

}