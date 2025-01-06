#ifndef MATH_VECTOR4_H
#define MATH_VECTOR4_H

#include <cmath>
#include <functional>
#include "Vector3.h"

namespace Math
{
	class Vector4 {
	public:
		float x, y, z, w;
		Vector4() :
			x(0),
			y(0),
			z(0),
			w(0)
		{};
		Vector4(float v1, float v2, float v3, float v4) {
			x = v1;
			y = v2;
			z = v3;
			w = v4;
		}
		Vector4(const Vector4& s) {
			x = s.x;
			y = s.y;
			z = s.z;
			w = s.w;
		}

		Vector4(const Vector4* _ptr) {
			x = _ptr->x;
			y = _ptr->y;
			z = _ptr->z;
			w = _ptr->w;
		}
		Vector4(const float* f_r) {
			x = f_r[0];
			y = f_r[1];
			z = f_r[2];
			w = f_r[3];
		}

		// Assignment operator from Vector3
		Vector4& operator=(const Vector3& v3) {
			x = v3.x;
			y = v3.y;
			z = v3.z;
			w = 1.0f;
			return *this;
		}

		Vector4& operator+=(const Vector4& b)
		{
			x += b.x;
			y += b.y;
			z += b.z;
			w += b.w;
			return *this;
		}

		// Vector * Vector
		Vector4 operator+(const Vector4& b) const
		{
			Vector4 ret = *this;
			ret += b;
			return ret;
		}

		// Vector: arithmetic updates
		Vector4& operator*=(float s)
		{
			x *= s;
			y *= s;
			z *= s;
			w *= s;
			return *this;
		}

		Vector4& operator*(float s)
		{
			Vector4 ret = this;
			ret.x *= s;
			ret.y *= s;
			ret.z *= s;
			ret.w *= s;
			return ret;
		}

		Vector4& operator-=(const Vector4& b)
		{
			x -= b.x;
			y -= b.y;
			z -= b.z;
			w -= b.w;
			return *this;
		}

		Vector4 operator-(const Vector4& b) const
		{
			Vector4 ret = *this;
			ret -= b;
			return ret;
		}



		bool operator==(const Vector4& v) const
		{
			return (x == v.x && y == v.y && z == v.z && w == v.w);
		}
		bool operator!=(const Vector4& v) const { return !(*this == v); }

		// check if vector is approximately the same
		bool equals(const Vector4& v, float epsilon = (float)1e-06) const {
			return (fabs(x - v.x) < epsilon && fabs(y - v.y) < epsilon && fabs(z - v.z) < epsilon && fabs(w - v.w) < epsilon);
		}

		Vector4 crossProduct(const Vector4& rkVector) const
		{
			return Vector4(
				y * rkVector.z - z * rkVector.y,
				z * rkVector.x - x * rkVector.z,
				x * rkVector.y - y * rkVector.x,
				w * rkVector.w - w * rkVector.w
				);
		}

		/** Calculates the dot (scalar) product of this vector with another.
			@remarks
				The dot product can be used to calculate the angle between 2
				vectors. If both are unit vectors, the dot product is the
				cosine of the angle; otherwise the dot product must be
				divided by the product of the lengths of both vectors to get
				the cosine of the angle. This result can further be used to
				calculate the distance of a point from a plane.
			@param
				vec Vector with which to calculate the dot product (together
				with this one).
			@return
				A float representing the dot product value.
		*/
		float dotProduct(const Vector4& vec) const
		{
			float ret = 0;

			ret += x * vec.x;
			ret += y * vec.y;
			ret += z * vec.z;
			ret += w * vec.w;
			return ret;
		}

		float squaredLength() const { return dotProduct(*this); }

		bool equals(const Vector4& vec) {
			Vector4 offset = *this - vec;
			return (offset.squaredLength() < 1e-005f * 1e-005f);
		}

		/** Returns the length (magnitude) of the vector.
			@warning
				This operation requires a square root and is expensive in
				terms of CPU operations. If you don't need to know the exact
				length (e.g. for just comparing lengths) use squaredLength()
				instead.
		*/
		float length() const { return sqrt(squaredLength()); }

		float normalize()
		{
			float fLength = length();

			if (fLength > float(0.0f))
			{
				float fInvLength = 1.0f / fLength;
				x *= fInvLength;
				y *= fInvLength;
				z *= fInvLength;
				w *= fInvLength;
			}

			return fLength;
		}

		/** As normalize, except that this vector is unaffected and the
			normalized vector is returned as a copy. */
		Vector4 normalizedCopy() const
		{
			Vector4 ret = *this;
			ret.normalize();
			return ret;
		}

		// helper

		// hashing helper
		size_t operator()(const Vector4& v) const {
			// You can use a simple hash combination technique
			size_t hash = 17;
			hash = hash * 31 + std::hash<float>{}(v.x);
			hash = hash * 31 + std::hash<float>{}(v.y);
			hash = hash * 31 + std::hash<float>{}(v.z);
			hash = hash * 31 + std::hash<float>{}(v.w);
			return hash;
		}
	};
}
#endif // !MATH_VECTOR4_H
