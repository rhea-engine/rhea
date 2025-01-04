#include "Interpolation.h"

namespace Math
{
    Math::Quaternion slerp(const Math::Quaternion& q1, const Math::Quaternion& q2, float t) {
        // Calculate the dot product (cosine of the angle)
        float dot = q1.dot(q2);

        // If the dot product is negative, slerp won't take the shorter path.
        // So, negate one quaternion to take the shorter path.
        Math::Quaternion q2Corrected = q2;
        if (dot < 0.0f) {
            q2Corrected = -q2;
            dot = -dot;
        }

        const float DOT_THRESHOLD = 0.9995f;
        if (dot > DOT_THRESHOLD) {
            // If the quaternions are nearly identical, use linear interpolation
            // to avoid division by zero errors.
            Math::Quaternion result = lerp(q1, q2Corrected, t);
            result.normalize(); // Normalize the result
            return result;
        }

        // Calculate the angle between the quaternions
        float theta_0 = acos(dot); //  = angle between input quaternions
        float theta = theta_0 * t; //  = angle at t
        float sin_theta = sin(theta); // Compute this once
        float sin_theta_0 = sin(theta_0); // Compute this once

        float s1 = cos(theta) - dot * sin_theta / sin_theta_0; // s1 = sin(th0 - θ) / sin(th0)
        float s2 = sin_theta / sin_theta_0; // s2 = sin(th) / sin(th0)

        return (q1 * s1) + (q2Corrected * s2);
    }
}