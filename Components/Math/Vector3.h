#ifndef MATH_VECTOR3_H
#define MATH_VECTOR3_H 1

#include <cmath>
#include "Quaternion.h"
#include <functional>

namespace Math
{
	class Vector3 {	
	public:
		float x,y,z;	
		Vector3() :
		x(0),
		y(0),
		z(0)
		{};
		Vector3(float v1, float v2, float v3) {
			x = v1;
			y = v2;
			z = v3;
		}
		Vector3(const Vector3& s) {
			x = s.x;
			y = s.y;
			z = s.z;
		}

		Vector3(const Vector3* _ptr) {
			x = _ptr->x;
			y = _ptr->y;
			z = _ptr->z;				
		}
		Vector3(const float* f_r) {
			x = f_r[0];
			y = f_r[1];
			z = f_r[2];
		}

		// special points
		static const Vector3 ZERO;
		static const Vector3 UNIT_X;
		static const Vector3 UNIT_Y;
		static const Vector3 UNIT_Z;
		static const Vector3 NEGATIVE_UNIT_X;
		static const Vector3 NEGATIVE_UNIT_Y;
		static const Vector3 NEGATIVE_UNIT_Z;
		static const Vector3 UNIT_SCALE;

		Vector3& operator+=(const Vector3& b)
		{
			x += b.x;
			y += b.y;
			z += b.z;
			return *this;
		}

		// Vector * Vector
		Vector3 operator+(const Vector3& b) const
		{
			Vector3 ret = *this;
			ret += b;
			return ret;
		}

		// Vector: arithmetic updates
		Vector3& operator*=(float s)
		{
			x *= s;
			y *= s;
			z *= s;
			return *this;
		}
		
		Vector3& operator*(float s)
		{
			Vector3 ret = this;
			ret.x *= s;
			ret.y *= s;
			ret.z *= s;
			return ret;
		}

		Vector3 operator/(float s)
		{
			Vector3 ret = this;
			ret.x /= s;
			ret.y /= s;
			ret.z /= s;
			return ret;
		}

		Vector3& operator-=(const Vector3& b)
		{
			x -= b.x;
			y -= b.y;
			z -= b.z;
			return *this;
		}

		Vector3 operator-(const Vector3& b) const
		{
			Vector3 ret = *this;
			ret -= b;
			return ret;
		}



		bool operator==(const Vector3& v) const
		{
			return (x == v.x && y == v.y && z == v.z);
		}
		bool operator!=(const Vector3& v) const { return !(*this == v); }

		// check if vector is approximately the same
		bool equals(const Vector3& v, float epsilon = (float)1e-06) const {
			return (fabs(x - v.x) < epsilon && fabs(y - v.y) < epsilon && fabs(z - v.z) < epsilon);
		}

		/**Gets the shortest arc quaternion to rotate this vector to the destination
			vector.
			@remarks
			If you call this with a dest vector that is close to the inverse
			of this vector, we will rotate 180 degrees around the 'fallbackAxis'
			(if specified, or a generated axis if not) since in this case
			ANY axis of rotation is valid.
		*/		
		//Quaternion getRotationTo(const Vector3 & dest, const Vector3 & fallbackAxis = ZERO) const;

		Quaternion getRotationTo(const Vector3& dest, const Vector3& fallbackAxis = ZERO) const
		{
			// From Sam Hocevar's article "Quaternion from two vectors:
			// the final version"
			float a = sqrt(((const Vector3*)this)->squaredLength() * dest.squaredLength());
			float b = a + dest.dotProduct(*this);
			Vector3 axis;

			if (b < (float)1e-06 * a)
			{
				b = (float)0.0;
				axis = fallbackAxis != Vector3::ZERO ? fallbackAxis
					: abs(x) > abs(z) ? Vector3(-y, x, (float)0.0)
					: Vector3((float)0.0, -z, y);
			}
			else
			{
				axis = this->crossProduct(dest);
			}

			Quaternion q(b, axis.x, axis.y, axis.z);
			//q.normalise();
			return q;
		}

		Vector3 crossProduct(const Vector3& rkVector) const
		{
			return Vector3(
				y * rkVector.z - z * rkVector.y,
				z * rkVector.x - x * rkVector.z,
				x * rkVector.y - y * rkVector.x);
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
		float dotProduct(const Vector3& vec) const
		{
			float ret = 0;

			ret += x * vec.x;
			ret += y * vec.y; 
			ret += z * vec.z;

			return ret;
		}

		float squaredLength() const { return dotProduct(*this); }

		bool equals(const Vector3& vec) {
			Vector3 offset = *this - vec;
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

		float normalise()
		{
			float fLength = length();
		
			if (fLength > float(0.0f))
			{
				float fInvLength = 1.0f / fLength;
				x *= fInvLength;
				y *= fInvLength;
				z *= fInvLength;
			}

			return fLength;
		}

		/** As normalise, except that this vector is unaffected and the
			normalised vector is returned as a copy. */
		Vector3 normalisedCopy() const
		{
			Vector3 ret = *this;
			ret.normalise();
			return ret;
		}		

		// helper

		// hashing helper
		size_t operator()(const Vector3 & v) const {
			// You can use a simple hash combination technique
			size_t hash = 17;
			hash = hash * 31 + std::hash<float>{}(v.x);
			hash = hash * 31 + std::hash<float>{}(v.y);
			hash = hash * 31 + std::hash<float>{}(v.z);
			return hash;
		}
	};
}
#endif // !MATH_VECTOR3_H
