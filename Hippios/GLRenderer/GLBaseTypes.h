#ifndef HIPPIOS_GL_BASETYPES_H
#define HIPPIOS_GL_BASETYPES_H

#include <Hippios/BaseTypes.h>
#include <unordered_map>
#include "GLIncludes.h"

namespace hippios
{
	struct GLGPUHandle : public GPUHandle
	{
		GLuint handle = 0;
	};

	extern std::unordered_map<ComparisonFunction, GLenum> gl_comparison_functions;
}

#endif // !HIPPIOS_GL_BASETYPES_H