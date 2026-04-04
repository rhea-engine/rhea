#include "Matrix3.h"
#include "Quaternion.h"
#include <cmath>

namespace math
{
	Matrix3 Quaternion::RotationMatrix() const
	{
		Matrix3 result;

		float epsilon = 1e-6f; // Threshold for floating-point precision

		float xx = x * x, yy = y * y, zz = z * z;
		float xy = x * y, xz = x * z, yz = y * z;
		float wx = w * x, wy = w * y, wz = w * z;

		result.m[0][0] = 1.0f - 2.0f * (yy + zz);
		result.m[0][1] = 2.0f * (xy - wz);
		result.m[0][2] = 2.0f * (xz + wy);

		result.m[1][0] = 2.0f * (xy + wz);
		result.m[1][1] = 1.0f - 2.0f * (xx + zz);
		result.m[1][2] = 2.0f * (yz - wx);

		result.m[2][0] = 2.0f * (xz - wy);
		result.m[2][1] = 2.0f * (yz + wx);
		result.m[2][2] = 1.0f - 2.0f * (xx + yy);

		// Clamp small values to 0
		for (int i = 0; i < 3; ++i) {
			for (int j = 0; j < 3; ++j) {
				if (std::abs(result.m[i][j]) < epsilon) {
					result.m[i][j] = 0.0f;
				}
			}
		}

		return result;
	}

	void Quaternion::fromRotationMatrix(const Matrix3& rot)
	{
		// Algorithm in Ken Shoemake's article in 1987 SIGGRAPH course notes
		// article "Quaternion Calculus and Fast Animation".

		float f_trace = rot[0][0] + rot[1][1] + rot[2][2];
		float f_root;

		if (f_trace > 0.0)
		{
			// |w| > 1/2, may as well choose w > 1/2
			f_root = sqrt(f_trace + 1.0f);  // 2w
			w = 0.5f * f_root;
			f_root = 0.5f / f_root;  // 1/(4w)
			x = (rot[2][1] - rot[1][2]) * f_root;
			y = (rot[0][2] - rot[2][0]) * f_root;
			z = (rot[1][0] - rot[0][1]) * f_root;
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

			f_root = sqrt(rot[i][i] - rot[j][j] - rot[k][k] + 1.0f);
			float* apkQuat[3] = { &x, &y, &z };
			*apkQuat[i] = 0.5f * f_root;
			f_root = 0.5f / f_root;
			w = (rot[k][j] - rot[j][k]) * f_root;
			*apkQuat[j] = (rot[j][i] + rot[i][j]) * f_root;
			*apkQuat[k] = (rot[k][i] + rot[i][k]) * f_root;
		}
	}

	void Quaternion::fromAngleAxis(const float& rfAngle,
		const Vector3& rkAxis)
	{
		// assert:  axis[] is unit length
		//
		// The quaternion representing the rotation is
		//   q = cos(A/2)+sin(A/2)*(x*i+y*j+z*k)

		float fHalfAngle((float)0.5 * rfAngle);
		float fSin = sin(fHalfAngle);
		w = cos(fHalfAngle);
		x = fSin * rkAxis.x;
		y = fSin * rkAxis.y;
		z = fSin * rkAxis.z;
	}

	void Quaternion::fromAxes(const Vector3* axis)
	{
		Matrix3 rot;

		for (size_t i_col = 0; i_col < 3; i_col++)
		{
			rot[0][i_col] = axis[i_col].x;
			rot[1][i_col] = axis[i_col].y;
			rot[2][i_col] = axis[i_col].z;
		}

		fromRotationMatrix(rot);
	}
	//-----------------------------------------------------------------------
	void Quaternion::fromAxes(const Vector3& x_axis, const Vector3& y_axis, const Vector3& z_axis)
	{
		Matrix3 rot;
		rot.FromAxes(x_axis, y_axis, z_axis);
		fromRotationMatrix(rot);
	}
	
	Vector3 Quaternion::operator* (const Vector3& v) const
	{
		// nVidia SDK implementation
		Vector3 uv, uuv;
		Vector3 q_vec(x, y, z);
		uv = q_vec.crossProduct(v);
		uuv = q_vec.crossProduct(uv);
		uv *= (2.0f * w);
		uuv *= 2.0f;

		return v + uv + uuv;
	}

	Quaternion Quaternion::operator* (const Quaternion& r_q) const
	{
		// C = A * B 
		// C.x = | A.w*B.x + A.x*B.w + A.y*B.z - A.z*B.y | 
		// C.y = | A.w*B.y - A.x*B.z + A.y*B.w + A.z*B.x | 
		// C.z = | A.w*B.z + A.x*B.y - A.y*B.x + A.z*B.w | 
		// C.w = | A.w*B.w - A.x*B.x - A.y*B.y - A.z*B.z |
		return Quaternion
		(
			w * r_q.w - x * r_q.x - y * r_q.y - z * r_q.z,
			w * r_q.x + x * r_q.w + y * r_q.z - z * r_q.y,
			w * r_q.y + y * r_q.w + z * r_q.x - x * r_q.z,
			w * r_q.z + z * r_q.w + x * r_q.y - y * r_q.x
		);
	}

	float Quaternion::normalize(void)
	{
		// compute old length
		float length = std::sqrt(x * x + y * y + z * z + w * w);

		// normalize
		if (length > 0.0f) {
			x = x / length;
			y = y / length;
			z = z / length;
			w = w / length;
		}

		return length;
	}

	void Quaternion::toRotationMatrix(Matrix3& rot) const
	{
		float f_Tx = x + x;
		float f_Ty = y + y;
		float f_Tz = z + z;
		float f_Twx = f_Tx * w;
		float f_Twy = f_Ty * w;
		float f_Twz = f_Tz * w;
		float f_Txx = f_Tx * x;
		float f_Txy = f_Ty * x;
		float f_Txz = f_Tz * x;
		float f_Tyy = f_Ty * y;
		float f_Tyz = f_Tz * y;
		float f_Tzz = f_Tz * z;

		rot[0][0] = 1.0f - (f_Tyy + f_Tzz);
		rot[0][1] = f_Txy - f_Twz;
		rot[0][2] = f_Txz + f_Twy;
		rot[1][0] = f_Txy + f_Twz;
		rot[1][1] = 1.0f - (f_Txx + f_Tzz);
		rot[1][2] = f_Tyz - f_Twx;
		rot[2][0] = f_Txz - f_Twy;
		rot[2][1] = f_Tyz + f_Twx;
		rot[2][2] = 1.0f - (f_Txx + f_Tyy);
	}

	void Quaternion::getAsAngleAxis(float& angle, Vector3& axis) const
	{
		Quaternion q = *this;
		q.normalize();
	
		angle = 2.0f * acos(q.w);

		// Avoid division by zero
		float sinHalfAngle = sqrt(1.0f - q.w * q.w);
		if (sinHalfAngle < 0.0001f) {
			// If the angle is close to zero, any axis will do
			axis = Vector3(1.0f, 0.0f, 0.0f);
		}
		else {
			// Compute the axis			
			axis = Vector3(q.x, q.y, q.z) / sinHalfAngle;
		}			
	}

	void Quaternion::toAxes(Vector3& x_axis, Vector3& y_axis, Vector3& z_axis) const
	{
		Matrix3 rot;
		toRotationMatrix(rot);

		x_axis.x = rot[0][0];
		x_axis.y = rot[0][1];
		x_axis.z = rot[0][2];

		y_axis.x = rot[1][0];
		y_axis.y = rot[1][1];
		y_axis.z = rot[1][2];

		z_axis.x = rot[2][0];
		z_axis.y = rot[2][1];
		z_axis.z = rot[2][2];

		x_axis.normalize();
		y_axis.normalize();
		z_axis.normalize();
	}

}