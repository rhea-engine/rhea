#include "Matrix3.h"
#include "Quaternion.h"

namespace Poseidon
{
	Matrix3 Quaternion::RotationMatrix() const
	{
		return Matrix3(
			1.0f - 2.0f * y * y - 2.0f * z * z,
			2.0f * x * y - 2.0f * w * z,
			2.0f * x * z + 2.0f * w * y,
			2.0f * x * y + 2.0f * w * z,
			1.0f - 2.0f * x * x - 2.0f * z * z,
			2.0f * y * z - 2.0f * w * x,
			2.0f * x * z - 2.0f * w * y,
			2.0f * y * z + 2.0f * w * x,
			1.0f - 2.0f * x * x - 2.0f * y * y
		);
	}

	Vector3 Quaternion::operator* (const Vector3& v) const
	{
		// nVidia SDK implementation
		Vector3 uv, uuv;
		Vector3 qvec(x, y, z);
		uv = qvec.crossProduct(v);
		uuv = qvec.crossProduct(uv);
		uv *= (2.0f * w);
		uuv *= 2.0f;

		return v + uv + uuv;

	}

	void Quaternion::FromRotationMatrix(const Matrix3& rot)
	{
		// Algorithm in Ken Shoemake's article in 1987 SIGGRAPH course notes
		// article "Quaternion Calculus and Fast Animation".

		float fTrace = rot[0][0] + rot[1][1] + rot[2][2];
		float fRoot;

		if (fTrace > 0.0)
		{
			// |w| > 1/2, may as well choose w > 1/2
			fRoot = sqrt(fTrace + 1.0f);  // 2w
			w = 0.5f * fRoot;
			fRoot = 0.5f / fRoot;  // 1/(4w)
			x = (rot[2][1] - rot[1][2]) * fRoot;
			y = (rot[0][2] - rot[2][0]) * fRoot;
			z = (rot[1][0] - rot[0][1]) * fRoot;
		}
		else
		{
			// |w| <= 1/2
			static size_t s_iNext[3] = { 1, 2, 0 };
			size_t i = 0;
			if (rot[1][1] > rot[0][0])
				i = 1;
			if (rot[2][2] > rot[i][i])
				i = 2;
			size_t j = s_iNext[i];
			size_t k = s_iNext[j];

			fRoot = sqrt(rot[i][i] - rot[j][j] - rot[k][k] + 1.0f);
			float* apkQuat[3] = { &x, &y, &z };
			*apkQuat[i] = 0.5f * fRoot;
			fRoot = 0.5f / fRoot;
			w = (rot[k][j] - rot[j][k]) * fRoot;
			*apkQuat[j] = (rot[j][i] + rot[i][j]) * fRoot;
			*apkQuat[k] = (rot[k][i] + rot[i][k]) * fRoot;
		}
	}

	void Quaternion::FromAngleAxis(const float& rfAngle,
		const Vector3& rkAxis)
	{
		// assert:  axis[] is unit length
		//
		// The quaternion representing the rotation is
		//   q = cos(A/2)+sin(A/2)*(x*i+y*j+z*k)

		float fHalfAngle(0.5 * rfAngle);
		float fSin = sin(fHalfAngle);
		w = cos(fHalfAngle);
		x = fSin * rkAxis.x;
		y = fSin * rkAxis.y;
		z = fSin * rkAxis.z;
	}

	void Quaternion::FromAxes(const Vector3* akAxis)
	{
		Matrix3 rot;

		for (size_t iCol = 0; iCol < 3; iCol++)
		{
			rot[0][iCol] = akAxis[iCol].x;
			rot[1][iCol] = akAxis[iCol].y;
			rot[2][iCol] = akAxis[iCol].z;
		}

		FromRotationMatrix(rot);
	}
	//-----------------------------------------------------------------------
	void Quaternion::FromAxes(const Vector3& xaxis, const Vector3& yaxis, const Vector3& zaxis)
	{
		Matrix3 rot;
		rot.FromAxes(xaxis, yaxis, zaxis);
		FromRotationMatrix(rot);
	}
	//-----------------------------------------------------------------------
	Quaternion Quaternion::operator* (const Quaternion& rkQ) const
	{
		return Quaternion
		(
			w * rkQ.w - x * rkQ.x - y * rkQ.y - z * rkQ.z,
			w * rkQ.x + x * rkQ.w + y * rkQ.z - z * rkQ.y,
			w * rkQ.y + y * rkQ.w + z * rkQ.x - x * rkQ.z,
			w * rkQ.z + z * rkQ.w + x * rkQ.y - y * rkQ.x
		);
	}

	float Quaternion::normalise(void)
	{
		return 0;
	}

	void Quaternion::ToRotationMatrix(Matrix3& rot) const
	{
		float fTx = x + x;
		float fTy = y + y;
		float fTz = z + z;
		float fTwx = fTx * w;
		float fTwy = fTy * w;
		float fTwz = fTz * w;
		float fTxx = fTx * x;
		float fTxy = fTy * x;
		float fTxz = fTz * x;
		float fTyy = fTy * y;
		float fTyz = fTz * y;
		float fTzz = fTz * z;

		rot[0][0] = 1.0f - (fTyy + fTzz);
		rot[0][1] = fTxy - fTwz;
		rot[0][2] = fTxz + fTwy;
		rot[1][0] = fTxy + fTwz;
		rot[1][1] = 1.0f - (fTxx + fTzz);
		rot[1][2] = fTyz - fTwx;
		rot[2][0] = fTxz - fTwy;
		rot[2][1] = fTyz + fTwx;
		rot[2][2] = 1.0f - (fTxx + fTyy);
	}
}