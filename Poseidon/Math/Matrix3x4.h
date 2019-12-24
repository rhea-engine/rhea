#ifndef POSEIDON_MATRIX3X4_H
#define POSEIDON_MATRIX3X4_H

#include "Matrix3.h"
#include "Quaternion.h"

namespace Poseidon
{
	class Matrix3x4
	{
	protected:
		float m[3][4];
	public:
		// default initialize with identity matrix
		Matrix3x4() :
			m{
				{ 1.0f, 0.0f, 0.0f, 0.0f },
				{ 0.0f, 1.0f, 0.0f, 0.0f },
				{ 0.0f, 0.0f, 1.0f, 0.0f }}
		{}

		// construct from other matrix
		Matrix3x4(const Matrix3x4& matrix) :
			m{
				{ matrix[0][0], matrix[0][1], matrix[0][2], matrix[0][3]},
				{ matrix[1][0], matrix[1][1], matrix[1][2], matrix[1][3]},
				{ matrix[2][0], matrix[2][1], matrix[2][2], matrix[2][3]} }
		{}

		// construct from values
		Matrix3x4(float v00, float v01, float v02, float v03,
			float v10, float v11, float v12, float v13,
			float v20, float v21, float v22, float v23) :
			m{
				{ v00, v01, v02, v03 },
				{ v10, v11, v12, v13 },
				{ v20, v21, v22, v23 }
		}
		{}

		static const Matrix3x4 IDENTITY;

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

		Matrix3x4& operator=(const Matrix3& mat3) {
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

		/// Set translation elements.
		void SetTranslation(const Vector3& translation)
		{
			m[0][3] = translation.x;
			m[1][3] = translation.y;
			m[2][3] = translation.z;
		}

		/// Set rotation elements from a 3x3 matrix.
		void SetRotation(const Matrix3& rotation)
		{
			m[0][0] = rotation.m[0][0];
			m[0][1] = rotation.m[0][1];
			m[0][2] = rotation.m[0][2];
			m[1][0] = rotation.m[1][0];
			m[1][1] = rotation.m[1][1];
			m[1][2] = rotation.m[1][2];
			m[2][0] = rotation.m[2][0];
			m[2][1] = rotation.m[2][1];
			m[2][2] = rotation.m[2][2];
		}

		/// Construct from translation, rotation and uniform scale.
		Matrix3x4(const Vector3& translation, const Quaternion& rotation, float scale) noexcept
		{
			SetRotation(rotation.RotationMatrix() * scale);
			SetTranslation(translation);
		}

		/// Construct from translation, rotation and nonuniform scale.
		Matrix3x4(const Vector3& translation, const Quaternion& rotation, const Vector3& scale) noexcept
		{
			SetRotation(rotation.RotationMatrix().Scaled(scale));
			SetTranslation(translation);
		}

		void Transform(const Vector3& translation, const Quaternion& rotation, const Vector3& scale) noexcept
		{
			SetRotation(rotation.RotationMatrix().Scaled(scale));
			SetTranslation(translation);
		}

		/// Assign from another matrix.
		Matrix3x4& operator =(const Matrix3x4& rhs) noexcept = default;

		/// Return the translation part.
		Vector3 Translation() const
		{
			return Vector3(
				m[0][3],
				m[1][3],
				m[2][3]
			);
		}
	};
	
	inline Matrix3x4 operator*(const Matrix3x4& m, const Matrix3x4& m2)
	{
		return Matrix3x4(
			m[0][0] * m2[0][0] + m[0][1] * m2[1][0] + m[0][2] * m2[2][0],
			m[0][0] * m2[0][1] + m[0][1] * m2[1][1] + m[0][2] * m2[2][1],
			m[0][0] * m2[0][2] + m[0][1] * m2[1][2] + m[0][2] * m2[2][2],
			m[0][0] * m2[0][3] + m[0][1] * m2[1][3] + m[0][2] * m2[2][3] + m[0][3],

			m[1][0] * m2[0][0] + m[1][1] * m2[1][0] + m[1][2] * m2[2][0],
			m[1][0] * m2[0][1] + m[1][1] * m2[1][1] + m[1][2] * m2[2][1],
			m[1][0] * m2[0][2] + m[1][1] * m2[1][2] + m[1][2] * m2[2][2],
			m[1][0] * m2[0][3] + m[1][1] * m2[1][3] + m[1][2] * m2[2][3] + m[1][3],

			m[2][0] * m2[0][0] + m[2][1] * m2[1][0] + m[2][2] * m2[2][0],
			m[2][0] * m2[0][1] + m[2][1] * m2[1][1] + m[2][2] * m2[2][1],
			m[2][0] * m2[0][2] + m[2][1] * m2[1][2] + m[2][2] * m2[2][2],
			m[2][0] * m2[0][3] + m[2][1] * m2[1][3] + m[2][2] * m2[2][3] + m[2][3]);
	}
}

#endif // !POSEIDON_MATRIX3X4_H
