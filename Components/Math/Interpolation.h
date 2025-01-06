// Interpolation.h

#pragma once

#include "Math/Vector3.h" 
#include "Math/Quaternion.h"

namespace Math
{
	template<typename T>
	T lerp(const T& a, const T& b, float t)
	{
		return a + t * (b - a);
	}

	Math::Quaternion slerp(const Math::Quaternion& q1, const Math::Quaternion& q2, float t);

}