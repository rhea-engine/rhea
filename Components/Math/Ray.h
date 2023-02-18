#ifndef MATH_RAY_H
#define MATH_RAY_H

#include "Vector3.h"

namespace Math
{
	class Ray {
	public:
		Ray(Vector3 origin, Vector3 direction) :
			m_origin(origin),
			m_direction(direction)
		{}

		Vector3 getOrigin() { return m_origin; }
		Vector3 getDirection() { return m_direction; }

	protected:
		Vector3 m_origin;
		Vector3 m_direction;
	};
}
#endif // !MATH_RAY_H
