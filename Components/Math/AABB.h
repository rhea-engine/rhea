#ifndef MATH_AABB_H
#define MATH_AABB_H

#include <Math/Vector3.h>
#include <Math/Matrix4.h>
#include <limits>

namespace math
{
	class AABB
	{
	public:
		AABB()
		{
			constexpr float inf = std::numeric_limits<float>::infinity();
			m_min = Vector3(inf, inf, inf);
			m_max = Vector3(-inf, -inf, -inf);
		}
		AABB(const Vector3& min, const Vector3& max) : m_min(min), m_max(max) {}
		const Vector3& getMin() const { return m_min; }
		const Vector3& getMax() const { return m_max; }
		void setMin(const Vector3& min) { m_min = min; }
		void setMax(const Vector3& max) { m_max = max; }
		bool contains(const Vector3& point) const {
			return (point.x >= m_min.x && point.x <= m_max.x &&
				point.y >= m_min.y && point.y <= m_max.y &&
				point.z >= m_min.z && point.z <= m_max.z);
		}
		void expand(const Vector3& point) {
			if (point.x < m_min.x) m_min.x = point.x;
			if (point.y < m_min.y) m_min.y = point.y;
			if (point.z < m_min.z) m_min.z = point.z;
			if (point.x > m_max.x) m_max.x = point.x;
			if (point.y > m_max.y) m_max.y = point.y;
			if (point.z > m_max.z) m_max.z = point.z;
		}
		void expand(const AABB& other) {
			if (other.isEmpty()) 
				return;
			expand(other.getMin());
			expand(other.getMax());
		}
		bool isEmpty() const {
			return (m_min.x > m_max.x || m_min.y > m_max.y || m_min.z > m_max.z);
		}
		Vector3 center() const {
			return (m_min + m_max) * 0.5f;
		}
		Vector3 size() const {
			return m_max - m_min;
		}
		AABB transformed(const Matrix4& mat) const
		{
			if (isEmpty())
				return AABB();
			Vector3 corners[8] = {
				Vector3(m_min.x, m_min.y, m_min.z),
				Vector3(m_max.x, m_min.y, m_min.z),
				Vector3(m_min.x, m_max.y, m_min.z),
				Vector3(m_max.x, m_max.y, m_min.z),
				Vector3(m_min.x, m_min.y, m_max.z),
				Vector3(m_max.x, m_min.y, m_max.z),
				Vector3(m_min.x, m_max.y, m_max.z),
				Vector3(m_max.x, m_max.y, m_max.z)
			};
			AABB result;
			for (int i = 0; i < 8; ++i) {
				Vector3 transformed_corner = mat * corners[i];
				result.expand(transformed_corner);
			}
			return result;
		}
	private:
		Vector3 m_min;
		Vector3 m_max;
	};
}

#endif // !MATH_AABB_H