#ifndef MATH_MATRIX4_H
#define MATH_MATRIX4_H

#include "Matrix3.h"
#include "Matrix3x4.h"
#include "Vector4.h"

namespace Math
{
	class Matrix4
	{
	protected:
		float m[4][4];
	public:
		// default initialize with identity matrix
		Matrix4() :
			m{ 
				{ 1.0f, 0.0f, 0.0f, 0.0f },
				{ 0.0f, 1.0f, 0.0f, 0.0f },
				{ 0.0f, 0.0f, 1.0f, 0.0f },
				{ 0.0f, 0.0f, 0.0f, 1.0f } }
		{}

		static const Matrix4 ZERO;
		static const Matrix4 IDENTITY;

		// construct from other matrix
		Matrix4(const Matrix4& matrix) :
			m{
				{ matrix[0][0], matrix[0][1], matrix[0][2], matrix[0][3]},
				{ matrix[1][0], matrix[1][1], matrix[1][2], matrix[1][3]},
				{ matrix[2][0], matrix[2][1], matrix[2][2], matrix[2][3]},
				{ matrix[3][0], matrix[3][1], matrix[3][2], matrix[3][3]}}
		{}
		Matrix4(const Matrix3x4& matrix) :
			m{
				{ matrix[0][0], matrix[0][1], matrix[0][2], matrix[0][3]},
				{ matrix[1][0], matrix[1][1], matrix[1][2], matrix[1][3]},
				{ matrix[2][0], matrix[2][1], matrix[2][2], matrix[2][3]},
				{ 0.0f, 0.0f, 0.0f, 1.0f } }
		{}

		// construct from values
		Matrix4(float v00, float v01, float v02, float v03,
			float v10, float v11, float v12, float v13,
			float v20, float v21, float v22, float v23,
			float v30, float v31, float v32, float v33) :
			m{
				{ v00, v01, v02, v03 },
				{ v10, v11, v12, v13 },
				{ v20, v21, v22, v23 },
				{ v30, v31, v32, v33 }
		}
		{}
		
		// Constructor to create an identity matrix
		Matrix4(float diagonal) {
			for (int i = 0; i < 4; ++i) {
				for (int j = 0; j < 4; ++j) {
					if (i == j) {
						m[i][j] = diagonal;  // Set diagonal elements to 1.0f
					}
					else {
						m[i][j] = 0.0f;      // Set all other elements to 0.0f
					}
				}
			}
		}

		Matrix4(const float* values) {
			for (int col = 0; col < 4; ++col) {      // Column-major order
				for (int row = 0; row < 4; ++row) {
					m[row][col] = values[col * 4 + row]; // Transpose into row-major order
				}
			}
		}

		// Constructor from a raw pointer to 16 floats or doubles (e.g., GLTF file data)
		explicit Matrix4(const double* values) {
			for (int col = 0; col < 4; ++col) {
				for (int row = 0; row < 4; ++row) {
					m[row][col] = static_cast<float>(values[col * 4 + row]); // Transpose
				}
			}
		}

		float* operator[](size_t row)
		{
			return m[row];
		}

		const float* operator[](size_t row) const
		{
			return m[row];
		}	

		/** Assignment from 3x3 matrix.
		*/		
		void set3x3Matrix(const Matrix3& mat3)
		{
			m[0][0] = mat3[0][0]; m[0][1] = mat3[0][1]; m[0][2] = mat3[0][2];
			m[1][0] = mat3[1][0]; m[1][1] = mat3[1][1]; m[1][2] = mat3[1][2];
			m[2][0] = mat3[2][0]; m[2][1] = mat3[2][1]; m[2][2] = mat3[2][2];
		}
		
		Matrix4& operator=(const Matrix3& mat3) {
			set3x3Matrix(mat3);
			return *this;
		}
		/** Extracts the rotation / scaling part of the Matrix as a 3x3 matrix.
		*/
		Matrix3 linear() const
		{
			return Matrix3(m[0][0], m[0][1], m[0][2],
				m[1][0], m[1][1], m[1][2],
				m[2][0], m[2][1], m[2][2]);
		}

		Matrix4 transpose() const;

		Matrix4 inverse() const;

		// Compose function
		static Matrix4 compose(const Vector3& translation, const Quaternion& rotation, const Vector3& scale) {
			
			// start with identity matrix
			Matrix4 result;

			// Get the 3x3 rotation matrix from the quaternion
			Matrix3 rotationMatrix = rotation.RotationMatrix();

			// Incorporate scale into the rotation matrix
			result.m[0][0] = rotationMatrix.m[0][0] * scale.x;
			result.m[0][1] = rotationMatrix.m[0][1] * scale.x;
			result.m[0][2] = rotationMatrix.m[0][2] * scale.x;

			result.m[1][0] = rotationMatrix.m[1][0] * scale.y;
			result.m[1][1] = rotationMatrix.m[1][1] * scale.y;
			result.m[1][2] = rotationMatrix.m[1][2] * scale.y;

			result.m[2][0] = rotationMatrix.m[2][0] * scale.z;
			result.m[2][1] = rotationMatrix.m[2][1] * scale.z;
			result.m[2][2] = rotationMatrix.m[2][2] * scale.z;

			// Add translation
			result.m[0][3] = translation.x;
			result.m[1][3] = translation.y;
			result.m[2][3] = translation.z;

			// Leave the last row as (0, 0, 0, 1) for a valid 4x4 matrix
			result.m[3][0] = 0.0f;
			result.m[3][1] = 0.0f;
			result.m[3][2] = 0.0f;
			result.m[3][3] = 1.0f;

			/*
			// Apply scale
			result.m[0][0] = scale.x;
			result.m[1][1] = scale.y;
			result.m[2][2] = scale.z;

			// Apply rotation using the 3x3 rotation matrix from the quaternion
			Matrix3 rotationMatrix = rotation.RotationMatrix();
			for (int i = 0; i < 3; ++i) {
				for (int j = 0; j < 3; ++j) {
					result.m[i][j] *= rotationMatrix.m[i][j]; // Incorporate the rotation
				}
			}

			// Apply translation
			result.m[0][3] = translation.x;
			result.m[1][3] = translation.y;
			result.m[2][3] = translation.z;
			*/

			return result;
		}

		bool isApproximatelyIdentity(float epsilon = 1e-4f) {
			for (int i = 0; i < 4; ++i) {
				for (int j = 0; j < 4; ++j) {
					if (i == j) {
						if (std::abs(m[i][j] - 1.0f) > epsilon) return false; // Diagonal should be 1
					}
					else {
						if (std::abs(m[i][j]) > epsilon) return false; // Non-diagonal should be 0
					}
				}
			}
			return true;
		}		
	};

	inline Matrix4 Matrix4::transpose() const
	{
		return Matrix4(m[0][0], m[1][0], m[2][0], m[3][0],
			m[0][1], m[1][1], m[2][1], m[3][1],
			m[0][2], m[1][2], m[2][2], m[3][2],
			m[0][3], m[1][3], m[2][3], m[3][3]);
	}

	inline Matrix4 operator*(const Matrix4& m, const Matrix4& m2)
	{
		Matrix4 r;
		r[0][0] = m[0][0] * m2[0][0] + m[0][1] * m2[1][0] + m[0][2] * m2[2][0] + m[0][3] * m2[3][0];
		r[0][1] = m[0][0] * m2[0][1] + m[0][1] * m2[1][1] + m[0][2] * m2[2][1] + m[0][3] * m2[3][1];
		r[0][2] = m[0][0] * m2[0][2] + m[0][1] * m2[1][2] + m[0][2] * m2[2][2] + m[0][3] * m2[3][2];
		r[0][3] = m[0][0] * m2[0][3] + m[0][1] * m2[1][3] + m[0][2] * m2[2][3] + m[0][3] * m2[3][3];

		r[1][0] = m[1][0] * m2[0][0] + m[1][1] * m2[1][0] + m[1][2] * m2[2][0] + m[1][3] * m2[3][0];
		r[1][1] = m[1][0] * m2[0][1] + m[1][1] * m2[1][1] + m[1][2] * m2[2][1] + m[1][3] * m2[3][1];
		r[1][2] = m[1][0] * m2[0][2] + m[1][1] * m2[1][2] + m[1][2] * m2[2][2] + m[1][3] * m2[3][2];
		r[1][3] = m[1][0] * m2[0][3] + m[1][1] * m2[1][3] + m[1][2] * m2[2][3] + m[1][3] * m2[3][3];

		r[2][0] = m[2][0] * m2[0][0] + m[2][1] * m2[1][0] + m[2][2] * m2[2][0] + m[2][3] * m2[3][0];
		r[2][1] = m[2][0] * m2[0][1] + m[2][1] * m2[1][1] + m[2][2] * m2[2][1] + m[2][3] * m2[3][1];
		r[2][2] = m[2][0] * m2[0][2] + m[2][1] * m2[1][2] + m[2][2] * m2[2][2] + m[2][3] * m2[3][2];
		r[2][3] = m[2][0] * m2[0][3] + m[2][1] * m2[1][3] + m[2][2] * m2[2][3] + m[2][3] * m2[3][3];

		r[3][0] = m[3][0] * m2[0][0] + m[3][1] * m2[1][0] + m[3][2] * m2[2][0] + m[3][3] * m2[3][0];
		r[3][1] = m[3][0] * m2[0][1] + m[3][1] * m2[1][1] + m[3][2] * m2[2][1] + m[3][3] * m2[3][1];
		r[3][2] = m[3][0] * m2[0][2] + m[3][1] * m2[1][2] + m[3][2] * m2[2][2] + m[3][3] * m2[3][2];
		r[3][3] = m[3][0] * m2[0][3] + m[3][1] * m2[1][3] + m[3][2] * m2[2][3] + m[3][3] * m2[3][3];

		return r;
	}

	inline Matrix4 Matrix4::inverse() const
	{
		Matrix4 out;

		float s[6];
		float c[6];
		s[0] = m[0][0] * m[1][1] - m[1][0] * m[0][1];
		s[1] = m[0][0] * m[1][2] - m[1][0] * m[0][2];
		s[2] = m[0][0] * m[1][3] - m[1][0] * m[0][3];
		s[3] = m[0][1] * m[1][2] - m[1][1] * m[0][2];
		s[4] = m[0][1] * m[1][3] - m[1][1] * m[0][3];
		s[5] = m[0][2] * m[1][3] - m[1][2] * m[0][3];

		c[0] = m[2][0] * m[3][1] - m[3][0] * m[2][1];
		c[1] = m[2][0] * m[3][2] - m[3][0] * m[2][2];
		c[2] = m[2][0] * m[3][3] - m[3][0] * m[2][3];
		c[3] = m[2][1] * m[3][2] - m[3][1] * m[2][2];
		c[4] = m[2][1] * m[3][3] - m[3][1] * m[2][3];
		c[5] = m[2][2] * m[3][3] - m[3][2] * m[2][3];

		/* is it invertible ? */
		float id = s[0] * c[5] - s[1] * c[4] + s[2] * c[3] + s[3] * c[2] - s[4] * c[1] + s[5] * c[0];
		if (id == 0.0f)
			return out;
		float idet = 1.0f / id ;

		out.m[0][0] = (m[1][1] * c[5] - m[1][2] * c[4] + m[1][3] * c[3]) * idet;
		out.m[0][1] = (-m[0][1] * c[5] + m[0][2] * c[4] - m[0][3] * c[3]) * idet;
		out.m[0][2] = (m[3][1] * s[5] - m[3][2] * s[4] + m[3][3] * s[3]) * idet;
		out.m[0][3] = (-m[2][1] * s[5] + m[2][2] * s[4] - m[2][3] * s[3]) * idet;

		out.m[1][0] = (-m[1][0] * c[5] + m[1][2] * c[2] - m[1][3] * c[1]) * idet;
		out.m[1][1] = (m[0][0] * c[5] - m[0][2] * c[2] + m[0][3] * c[1]) * idet;
		out.m[1][2] = (-m[3][0] * s[5] + m[3][2] * s[2] - m[3][3] * s[1]) * idet;
		out.m[1][3] = (m[2][0] * s[5] - m[2][2] * s[2] + m[2][3] * s[1]) * idet;

		out.m[2][0] = (m[1][0] * c[4] - m[1][1] * c[2] + m[1][3] * c[0]) * idet;
		out.m[2][1] = (-m[0][0] * c[4] + m[0][1] * c[2] - m[0][3] * c[0]) * idet;
		out.m[2][2] = (m[3][0] * s[4] - m[3][1] * s[2] + m[3][3] * s[0]) * idet;
		out.m[2][3] = (-m[2][0] * s[4] + m[2][1] * s[2] - m[2][3] * s[0]) * idet;

		out.m[3][0] = (-m[1][0] * c[3] + m[1][1] * c[1] - m[1][2] * c[0]) * idet;
		out.m[3][1] = (m[0][0] * c[3] - m[0][1] * c[1] + m[0][2] * c[0]) * idet;
		out.m[3][2] = (-m[3][0] * s[3] + m[3][1] * s[1] - m[3][2] * s[0]) * idet;
		out.m[3][3] = (m[2][0] * s[3] - m[2][1] * s[1] + m[2][2] * s[0]) * idet;

		return out;
	}

	inline Vector3 operator*(const Matrix4& m, const Vector3& v)
	{
		Vector3 r;

		float fInvW = 1.0f / (m[3][0] * v.x + m[3][1] * v.y + m[3][2] * v.z + m[3][3]);

		r.x = (m[0][0] * v.x + m[0][1] * v.y + m[0][2] * v.z + m[0][3]) * fInvW;
		r.y = (m[1][0] * v.x + m[1][1] * v.y + m[1][2] * v.z + m[1][3]) * fInvW;
		r.z = (m[2][0] * v.x + m[2][1] * v.y + m[2][2] * v.z + m[2][3]) * fInvW;

		return r;
	}

	inline Vector4 operator*(const Vector4& v, const Matrix4& m)
	{
		Vector4 r;
		r.x = v.x * m[0][0] + v.y * m[1][0] + v.z * m[2][0] + v.w * m[3][0];
		r.y = v.x * m[0][1] + v.y * m[1][1] + v.z * m[2][1] + v.w * m[3][1];
		r.z = v.x * m[0][2] + v.y * m[1][2] + v.z * m[2][2] + v.w * m[3][2];
		r.w = v.x * m[0][3] + v.y * m[1][3] + v.z * m[2][3] + v.w * m[3][3];
		return r;
	}
}

#endif // !MATH_MATRIX4_H
