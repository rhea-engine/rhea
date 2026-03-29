#include "GLBaseTypes.h"

namespace Hippios
{
	std::unordered_map<ComparisonFunction, GLenum> gl_comparison_functions = {
		{ComparisonFunction::COMPARE_NEVER, GL_NEVER},
		{ComparisonFunction::COMPARE_LESS, GL_LESS},
		{ComparisonFunction::COMPARE_EQUAL, GL_EQUAL},
		{ComparisonFunction::COMPARE_LESS_EQUAL, GL_LEQUAL},
		{ComparisonFunction::COMPARE_GREATER, GL_GREATER},
		{ComparisonFunction::COMPARE_NOT_EQUAL, GL_NOTEQUAL},
		{ComparisonFunction::COMPARE_GREATER_EQUAL, GL_GEQUAL},
		{ComparisonFunction::COMPARE_ALWAYS, GL_ALWAYS}
	};
}