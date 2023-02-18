#include "Matrix3.h"

namespace Math
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
}