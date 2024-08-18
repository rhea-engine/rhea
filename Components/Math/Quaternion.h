#ifndef MATH_QUATERNION_H
#define MATH_QUATERNION_H

namespace Math
{
	class Vector3;
	class Matrix3;

	// this class is based on Ogre3d's OgreQuaternion

	class Quaternion {
	public:
		float w, x, y, z;

		static const Quaternion IDENTITY;

		Quaternion() : w(1), x(0), y(0), z(0) {};
		
		// Copy constructor
		inline Quaternion(const Quaternion& rhs)
			: w(rhs.w), x(rhs.x), y(rhs.y), z(rhs.z)
		{}

		// Constructor from explizit values
		Quaternion(float iw, float ix, float iy, float iz) {
			x = ix;
			y = iy;
			z = iz;
			w = iw;
		}

		// Constructor for a quaternion from 3 orthonormal local axes
		inline Quaternion(const Vector3& x_axis, const Vector3& y_axis, const Vector3& z_axis)
		{
			this->fromAxes(x_axis, y_axis, z_axis);
		}

		// Constructor for a quaternion from an angle/axis
		inline Quaternion(const float& r_angle, const Vector3& r_axis)
		{
			this->fromAngleAxis(r_angle, r_axis);
		}

		// setter functions 
				
		// setup by the supplied vector, and "roll" around that vector by the specified radians.
		void fromAngleAxis(const float& r_angle, const Vector3& r_axis);

		// Setup from rotation matrix
		void fromRotationMatrix(const Matrix3& rot);
		
		// setup from axes in a vector
		void fromAxes(const Vector3* a_axis);
		
		// setup from axes in thre vectors
		void fromAxes(const Vector3& x_axis, const Vector3& y_axis, const Vector3& z_axis);

		// Normalises this quaternion, and returns the previous length
		float normalise(void);
		
		// operations

		// mulitply with other quaternion
		Quaternion operator* (const Quaternion& r_q) const;

		// Rotation of a vector by a quaternion
		Vector3 operator* (const Vector3& r_vector) const;

		// Dot product
		float dot(const Quaternion& other) const {
			return w * other.w + x * other.x + y * other.y + z * other.z;
		}

		// Negation
		Quaternion operator-() const {
			return Quaternion(-w, -x, -y, -z);
		}

		// Scalar multiplication
		Quaternion operator*(float scalar) const {
			return Quaternion(w * scalar, x * scalar, y * scalar, z * scalar);
		}

		// Quaternion addition operator
		Quaternion operator+(const Quaternion& other) const {
			return Quaternion(w + other.w, x + other.x, y + other.y, z + other.z);
		}

		// Quaternion subtraction operator
		Quaternion operator-(const Quaternion& other) const {
			return Quaternion(w - other.w, x - other.x, y - other.y, z - other.z);
		}

		void toRotationMatrix(Matrix3& rot) const;

		// getter functions

		// get as rotation matrix
		Matrix3 RotationMatrix() const;

		// get as angle/axis
		void getAsAngleAxis(float& angle, Vector3& axis) const;

		// get as vector axes
		void toAxes(Vector3& x_axis, Vector3& y_axis, Vector3& z_axis) const;
	};

	// Non-member function to handle float * Quaternion
	inline Quaternion operator*(float scalar, const Quaternion& quat) {
		return quat * scalar;
	}
}
#endif // !MATH_QUATERNION_H
