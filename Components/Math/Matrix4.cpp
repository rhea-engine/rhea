/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#include <Math/Matrix4.h>
#include <Math/Matrix3.h>
#include <Math/Quaternion.h>
#include <Math/Vector3.h>
#include <cmath>

namespace math
{
	const Matrix4 Matrix4::ZERO(
		0.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 0.0f);

	const Matrix4 Matrix4::IDENTITY(
		1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1);

	bool decomposeTRS(
		const math::Matrix4& matrix,
		math::Vector3& translation,
		math::Quaternion& rotation,
		math::Vector3& scale)
	{
        constexpr float epsilon = 1e-6f;
        constexpr float orthogonal_tolerance = 1e-4f;

        // Reject invalid values and non-affine matrices.
        for (int row = 0; row < 4; ++row) {
            for (int col = 0; col < 4; ++col) {
                if (!std::isfinite(matrix[row][col]))
                    return false;
            }
        }

        if (std::abs(matrix[3][0]) > epsilon ||
            std::abs(matrix[3][1]) > epsilon ||
            std::abs(matrix[3][2]) > epsilon ||
            std::abs(matrix[3][3] - 1.0f) > epsilon)
            return false;

        math::Vector3 x_axis(matrix[0][0], matrix[1][0], matrix[2][0]);
        math::Vector3 y_axis(matrix[0][1], matrix[1][1], matrix[2][1]);
        math::Vector3 z_axis(matrix[0][2], matrix[1][2], matrix[2][2]);

        math::Vector3 result_scale(x_axis.length(), y_axis.length(), z_axis.length());

        // This version rejects zero or near-zero scale.
        if (!std::isfinite(result_scale.x) ||
            !std::isfinite(result_scale.y) ||
            !std::isfinite(result_scale.z) ||
            result_scale.x <= epsilon ||
            result_scale.y <= epsilon ||
            result_scale.z <= epsilon)
            return false;

        x_axis *= 1.0f / result_scale.x;
        y_axis *= 1.0f / result_scale.y;
        z_axis *= 1.0f / result_scale.z;

        // TRS cannot represent shear.
        if (std::abs(x_axis.dotProduct(y_axis)) > orthogonal_tolerance ||
            std::abs(x_axis.dotProduct(z_axis)) > orthogonal_tolerance ||
            std::abs(y_axis.dotProduct(z_axis)) > orthogonal_tolerance)
            return false;

        // Put any reflection into the X scale.
        if (x_axis.dotProduct(y_axis.crossProduct(z_axis)) < 0.0f) {
            result_scale.x = -result_scale.x;
            x_axis *= -1.0f;
        }

        const math::Matrix3 rotation_matrix(
            x_axis.x, y_axis.x, z_axis.x,
            x_axis.y, y_axis.y, z_axis.y,
            x_axis.z, y_axis.z, z_axis.z);

        math::Quaternion result_rotation;
        result_rotation.fromRotationMatrix(rotation_matrix);
        result_rotation.normalize();

        // Only modify outputs after successful decomposition.
        translation = math::Vector3(matrix[0][3], matrix[1][3], matrix[2][3]);
        rotation = result_rotation;
        scale = result_scale;
        return true;
	}

}