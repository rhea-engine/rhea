#ifndef MATH_MATRIX3_H
#define MATH_MATRIX3_H 1

#include <string.h>

#include "Vector3.h"

namespace Math
{
	class Matrix3
	{
	public:
		float m[3][3];		
		Matrix3() :
			m{
				{ 1.0f, 0.0f, 0.0f },
				{ 0.0f, 1.0f, 0.0f },				
				{ 0.0f, 0.0f, 1.0f } }
		{}
		explicit Matrix3(const float m_arr[3][3])
		{
			memcpy(m, m_arr, 9 * sizeof(float));
		};

		Matrix3(float m00, float m01, float m02,
			float m10, float m11, float m12,
			float m20, float m21, float m22)
		{
			m[0][0] = m00;
			m[0][1] = m01;
			m[0][2] = m02;
			m[1][0] = m10;
			m[1][1] = m11;
			m[1][2] = m12;
			m[2][0] = m20;
			m[2][1] = m21;
			m[2][2] = m22;
		};

		/// Member access, allows use of construct mat[r][c]
		const float* operator[] (size_t iRow) const
		{
			return m[iRow];
		}

		float* operator[] (size_t iRow)
		{
			return m[iRow];
		}
		
		/** Matrix concatenation using '*'.
		 */
		Matrix3 operator* (const Matrix3& rkMatrix) const;
		Matrix3 operator- () const;

		void SetColumn(size_t col, const Vector3& vec);

		Matrix3 Transpose() const;

		void FromAxes(const Vector3& xAxis, const Vector3& yAxis, const Vector3& zAxis);

		/// Multiply with a scalar.
		Matrix3 operator *(float rhs) const
		{
			return Matrix3(
				m[0][0] * rhs,
				m[0][1] * rhs,
				m[0][2] * rhs,
				m[1][0] * rhs,
				m[1][1] * rhs,
				m[1][2] * rhs,
				m[2][0] * rhs,
				m[2][1] * rhs,
				m[2][2] * rhs
			);
		};

		/// Multiply a Vector3.
		Vector3 operator *(const Vector3& rhs) const
		{
			return Vector3(
				m[0][0] * rhs.x + m[0][1] * rhs.y + m[0][2] * rhs.z,
				m[1][0] * rhs.x + m[1][1] * rhs.y + m[1][2] * rhs.z,
				m[2][0] * rhs.x + m[2][1] * rhs.y + m[2][2] * rhs.z
			);
		};

		/// Return scaled by a vector.
		Matrix3 Scaled(const Vector3& scale) const
		{
			return Matrix3(
				m[0][0] * scale.x,
				m[0][1] * scale.y,
				m[0][2] * scale.z,
				m[1][0] * scale.x,
				m[1][1] * scale.y,
				m[1][2] * scale.z,
				m[2][0] * scale.x,
				m[2][1] * scale.y,
				m[2][2] * scale.z
			);
		};

		// Calculate the determinant of the matrix
		float Determinant() const {
			return m[0][0] * (m[1][1] * m[2][2] - m[1][2] * m[2][1]) -
				m[0][1] * (m[1][0] * m[2][2] - m[1][2] * m[2][0]) +
				m[0][2] * (m[1][0] * m[2][1] - m[1][1] * m[2][0]);
		}

		// Invert the matrix
		Matrix3 Inverse() const;
	};
}

#endif