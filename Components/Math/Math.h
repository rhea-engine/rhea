#ifndef MATH_H
#define MATH_H

namespace math
{
#ifndef M_PI
	static constexpr float M_PI = 3.14159265358979323846f;
#endif
	static constexpr float M_DEGTORAD = M_PI / 180.0f;
	static constexpr float M_RADTODEG = 1.0f / M_DEGTORAD;
}
#endif // !MATH_H
