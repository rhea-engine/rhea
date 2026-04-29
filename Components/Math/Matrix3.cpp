/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#include "Matrix3.h"

namespace math
{
	void Matrix3::SetColumn(size_t col, const Vector3& vec)
	{		
		m[0][col] = vec.x;
		m[1][col] = vec.y;
		m[2][col] = vec.z;

	}

	void Matrix3::FromAxes(const Vector3& xAxis, const Vector3& yAxis, const Vector3& zAxis)
	{
		SetColumn(0, xAxis);
		SetColumn(1, yAxis);
		SetColumn(2, zAxis);
	}

	Matrix3 Matrix3::Transpose() const
	{
		Matrix3 dest;
		for (size_t row = 0; row < 3; row++)
		{
			for (size_t col = 0; col < 3; col++)
				dest[row][col] = m[col][row];
		}
		return dest;
	}

	Matrix3 Matrix3::operator* (const Matrix3& rkMatrix) const
	{
		Matrix3 dest;
		for (size_t row = 0; row < 3; row++)
		{
			for (size_t col = 0; col < 3; col++)
			{
				dest.m[row][col] =
					m[row][0] * rkMatrix.m[0][col] +
					m[row][1] * rkMatrix.m[1][col] +
					m[row][2] * rkMatrix.m[2][col];
			}
		}
		return dest;
	}

	Matrix3 Matrix3::operator- () const
	{
		Matrix3 dest;
		for (size_t row = 0; row < 3; row++)
		{
			for (size_t col = 0; col < 3; col++)
				dest[row][col] = -m[row][col];
		}
		return dest;
	}

	Matrix3 Matrix3::Inverse() const {
		Matrix3 result;

		float det = Determinant();
		if (std::abs(det) < std::numeric_limits<float>::epsilon()) {
			// Matrix is singular, cannot invert			
			return result;
		}

		float invDet = 1.0f / det;

		result.m[0][0] = (m[1][1] * m[2][2] - m[1][2] * m[2][1]) * invDet;
		result.m[0][1] = (m[0][2] * m[2][1] - m[0][1] * m[2][2]) * invDet;
		result.m[0][2] = (m[0][1] * m[1][2] - m[0][2] * m[1][1]) * invDet;

		result.m[1][0] = (m[1][2] * m[2][0] - m[1][0] * m[2][2]) * invDet;
		result.m[1][1] = (m[0][0] * m[2][2] - m[0][2] * m[2][0]) * invDet;
		result.m[1][2] = (m[0][2] * m[1][0] - m[0][0] * m[1][2]) * invDet;

		result.m[2][0] = (m[1][0] * m[2][1] - m[1][1] * m[2][0]) * invDet;
		result.m[2][1] = (m[0][1] * m[2][0] - m[0][0] * m[2][1]) * invDet;
		result.m[2][2] = (m[0][0] * m[1][1] - m[0][1] * m[1][0]) * invDet;

		return result;
	}
}