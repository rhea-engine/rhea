#ifndef POSEIDON_MATRIX4_H
#define POSEIDON_MATRIX4_H

#include "Matrix3.h"

namespace Poseidon
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

		bool inverse(Matrix4& out);
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

	inline bool Matrix4::inverse(Matrix4 &out)
	{
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
			return false;
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

		return true;
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
}

#endif // !POSEIDON_MATRIX4_H
