#include <Math/Vector3.h>
#include <Math/Matrix3.h>
#include <cmath>
#include <iostream>

bool checkRotation(const math::Vector3& source, const math::Vector3& target) {
    const math::Quaternion rotation = source.getRotationTo(target);
    const float norm_squared = rotation.w * rotation.w + rotation.x * rotation.x
        + rotation.y * rotation.y + rotation.z * rotation.z;
    math::Matrix3 matrix;
    rotation.toRotationMatrix(matrix);
    const math::Vector3 expected = target.normalizedCopy();
    const math::Vector3 actual = matrix * source.normalizedCopy();
    return std::abs(norm_squared - 1.0f) < 1e-5f
        && (actual - expected).squaredLength() < 1e-8f;
}

int main() {
    if (!checkRotation(math::Vector3::UNIT_Z, math::Vector3(0, -2, 7.5f))
        || !checkRotation(math::Vector3::UNIT_Z, math::Vector3::UNIT_Z)
        || !checkRotation(math::Vector3::UNIT_Z, -math::Vector3::UNIT_Z)
        || !checkRotation(math::Vector3(3, 0, 0), math::Vector3(0, 4, 0))) {
        std::cerr << "getRotationTo must produce a unit quaternion pointing at the target.\n";
        return 1;
    }
    const math::Quaternion zero_rotation = math::Vector3::ZERO.getRotationTo(math::Vector3::UNIT_Z);
    if (zero_rotation.w != 1.0f || zero_rotation.x != 0.0f
        || zero_rotation.y != 0.0f || zero_rotation.z != 0.0f) {
        std::cerr << "A zero direction must leave the orientation unchanged.\n";
        return 1;
    }
    return 0;
}
