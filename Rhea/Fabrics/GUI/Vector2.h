#ifndef RHEA_GUI_VECTOR2_H
#define RHEA_GUI_VECTOR2_H

namespace Rhea
{
	class Vector2 {
	public:
		float x, y;

		Vector2() :
			x(0),
			y(0)			
		{};
		Vector2(float v1, float v2) {
			x = v1;
			y = v2;
		}
		Vector2(const Vector2& s) {
			x = s.x;
			y = s.y;
		}

		Vector2(const Vector2* _ptr) {
			x = _ptr->x;
			y = _ptr->y;
		}

		// operations on vector

		Vector2& operator+=(const Vector2& b)
		{
			x += b.x;
			y += b.y;
			return *this;
		}

		// Vector * Vector
		Vector2 operator+(const Vector2& b) const
		{
			Vector2 ret = *this;
			ret += b;
			return ret;
		}

		// Vector: arithmetic updates
		Vector2& operator*=(float s)
		{
			x *= s;
			y *= s;
			return *this;
		}

		Vector2& operator-=(const Vector2& b)
		{
			x -= b.x;
			y -= b.y;
			return *this;
		}

		Vector2 operator-(const Vector2& b) const
		{
			Vector2 ret = *this;
			ret -= b;
			return ret;
		}

		bool operator==(const Vector2& v) const
		{
			return (x == v.x && y == v.y);
		}
		bool operator!=(const Vector2& v) const { return !(*this == v); }
	};
}
#endif // !RHEA_GUI_VECTOR2_H
