/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#ifndef MATH_COLOR_H
#define MATH_COLOR_H

#include "Vector3.h"
#include "Vector4.h"
#include "Quaternion.h"

namespace math
{
	struct Color
	{
		float r, g, b, a;
		Color() : r(0), g(0), b(0), a(1) {}
		Color(float r, float g, float b, float a = 1.0f) : r(r), g(g), b(b), a(a) {}
		Color(const Vector3& rgb, float a) : r(rgb.x), g(rgb.y), b(rgb.z), a(a) {}
		Color(const Vector4& rgba) : r(rgba.x), g(rgba.y), b(rgba.z), a(rgba.w) {}
		Vector3 toVector3() const { return Vector3(r, g, b); }
		Vector4 toVector4() const { return Vector4(r, g, b, a); }
		Quaternion toQuaternion() const { return Quaternion(r, g, b, a); }	

		bool operator==(const Color& other) const {
			return (r == other.r && g == other.g && b == other.b && a == other.a);
		}

		bool operator!=(const Color& other) const {
			return !(*this == other);
		}

		static const Color WHITE;
		static const Color BLACK;
	};
}
#endif // !MATH_COLOR_H
