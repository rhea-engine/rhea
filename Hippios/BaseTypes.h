#ifndef HIPPIOS_BASETYPES_H
#define HIPPIOS_BASETYPES_H

namespace Hippios
{
	struct GPUHandle {};

	enum class ComparisonFunction
	{
		COMPARE_NEVER = 0,
		COMPARE_LESS = 1,
		COMPARE_EQUAL = 2,
		COMPARE_LESS_EQUAL = 3,
		COMPARE_GREATER = 4,
		COMPARE_NOT_EQUAL = 5,
		COMPARE_GREATER_EQUAL = 6,
		COMPARE_ALWAYS = 7
	};	
}
#endif // !HIPPIOS_BASETYPES_H
