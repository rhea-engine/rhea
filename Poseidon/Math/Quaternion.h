#ifndef POSEIDON_QUATERNION_H
#define POSEIDON_QUATERNION_H

namespace Poseidon
{
	class Vector3;
	class Matrix3;

	class Quaternion {
	public:
		float w, x, y, z;

		static const Quaternion IDENTITY;

		Quaternion() : w(1), x(0), y(0), z(0) {};
		
		/// Copy constructor
		inline Quaternion(const Quaternion& rhs)
			: w(rhs.w), x(rhs.x), y(rhs.y), z(rhs.z)
		{}

		Quaternion(float iw, float ix, float iy, float iz) {
			x = ix;
			y = iy;
			z = iz;
			w = iw;
		}

		Quaternion operator* (const Quaternion& rkQ) const;

		/// Construct a quaternion from 3 orthonormal local axes
		inline Quaternion(const Vector3& xaxis, const Vector3& yaxis, const Vector3& zaxis)
		{
			this->FromAxes(xaxis, yaxis, zaxis);
		}
		/** Setups the quaternion using the supplied vector, and "roll" around
		   that vector by the specified radians.
	   */
		void FromAngleAxis(const float& rfAngle, const Vector3& rkAxis);
		/// Construct a quaternion from an angle/axis
		inline Quaternion(const float& rfAngle, const Vector3& rkAxis)
		{
			this->FromAngleAxis(rfAngle, rkAxis);
		}
		/// Normalises this quaternion, and returns the previous length
		float normalise(void);
		void FromRotationMatrix(const Matrix3& kRot);
		void FromAxes(const Vector3* akAxis);
		void FromAxes(const Vector3& xAxis, const Vector3& yAxis, const Vector3& zAxis);

		Matrix3 RotationMatrix() const;
		/// Rotation of a vector by a quaternion
		Vector3 operator* (const Vector3& rkVector) const;
		void ToRotationMatrix(Matrix3& rot) const;
	};
}
#endif // !POSEIDON_QUATERNION_H
