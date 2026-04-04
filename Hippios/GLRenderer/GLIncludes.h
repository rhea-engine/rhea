#ifndef HIPPIOS_GL_INCLUDES_H
#define HIPPIOS_GL_INCLUDES_H

// Windows.h MUST come before glad.h to avoid APIENTRY redefinition warnings.
// glad.h checks if APIENTRY is already defined and skips its own definition.
#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#endif

// GLAD must be included BEFORE any other OpenGL header.
// It provides all GL types, constants, and function declarations
// for OpenGL 4.5 Core Profile.
#include <glad/glad.h>

#ifdef _WIN32
// WGL extension constants (for context creation)
// These are not part of GLAD but needed for wglCreateContextAttribsARB
#define WGL_CONTEXT_MAJOR_VERSION_ARB     0x2091
#define WGL_CONTEXT_MINOR_VERSION_ARB     0x2092
#define WGL_CONTEXT_FLAGS_ARB             0x2094
#define WGL_CONTEXT_PROFILE_MASK_ARB      0x9126
#define WGL_CONTEXT_CORE_PROFILE_BIT_ARB  0x00000001
#define WGL_CONTEXT_DEBUG_BIT_ARB         0x00000001

typedef HGLRC(WINAPI* PFNWGLCREATECONTEXTATTRIBSARBPROC)(HDC, HGLRC, const int*);
typedef BOOL(WINAPI* PFNWGLCHOOSEPIXELFORMATARBPROC)(HDC, const int*, const FLOAT*, UINT, int*, UINT*);
#endif

#include "Hippios/PipelineState.h"

namespace hippios
{
	struct GLCapabilities
	{
		int majorVersion = 0;
		int minorVersion = 0;

		bool hasDSA = false;          // modern direct state access path
		bool hasDebugOutput = false;
	};

	inline GLenum toGLTopology(PrimitiveTopology topo)
	{
		switch (topo) {
		case PrimitiveTopology::TriangleList: return GL_TRIANGLES;
		case PrimitiveTopology::TriangleStrip: return GL_TRIANGLE_STRIP;
		case PrimitiveTopology::LineList: return GL_LINES;
		case PrimitiveTopology::LineStrip: return GL_LINE_STRIP;
		case PrimitiveTopology::PointList: return GL_POINTS;
		default: return GL_TRIANGLES;
		}
	}

	inline static int getFormatComponentCount(InputLayoutFormat fmt)
	{
		switch (fmt)
		{
		case InputLayoutFormat::R32_FLOAT:
		case InputLayoutFormat::R32_UINT:
		case InputLayoutFormat::R32_SINT:
		case InputLayoutFormat::R16_FLOAT:
		case InputLayoutFormat::R16_UINT:
		case InputLayoutFormat::R16_SINT:
		case InputLayoutFormat::R8_UNORM:
		case InputLayoutFormat::R8_UINT:
		case InputLayoutFormat::R8_SINT:
			return 1;

		case InputLayoutFormat::R32G32_FLOAT:
		case InputLayoutFormat::R32G32_UINT:
		case InputLayoutFormat::R32G32_SINT:
		case InputLayoutFormat::R16G16_FLOAT:
		case InputLayoutFormat::R16G16_UINT:
		case InputLayoutFormat::R16G16_SINT:
		case InputLayoutFormat::R8G8_UNORM:
		case InputLayoutFormat::R8G8_UINT:
		case InputLayoutFormat::R8G8_SINT:
			return 2;

		case InputLayoutFormat::R32G32B32_FLOAT:
		case InputLayoutFormat::R32G32B32_UINT:
		case InputLayoutFormat::R32G32B32_SINT:
			return 3;

		case InputLayoutFormat::R32G32B32A32_FLOAT:
		case InputLayoutFormat::R32G32B32A32_UINT:
		case InputLayoutFormat::R32G32B32A32_SINT:
		case InputLayoutFormat::R16G16B16A16_FLOAT:
		case InputLayoutFormat::R16G16B16A16_UINT:
		case InputLayoutFormat::R16G16B16A16_SINT:
		case InputLayoutFormat::R8G8B8A8_UNORM:
		case InputLayoutFormat::R8G8B8A8_UINT:
		case InputLayoutFormat::R8G8B8A8_SINT:
			return 4;

		default:
			return 0;
		}
	}

	inline static GLenum getFormatType(InputLayoutFormat fmt)
	{
		switch (fmt)
		{
		case InputLayoutFormat::R32_FLOAT:
		case InputLayoutFormat::R32G32_FLOAT:
		case InputLayoutFormat::R32G32B32_FLOAT:
		case InputLayoutFormat::R32G32B32A32_FLOAT:
			return GL_FLOAT;

		case InputLayoutFormat::R32_UINT:
		case InputLayoutFormat::R32G32_UINT:
		case InputLayoutFormat::R32G32B32_UINT:
		case InputLayoutFormat::R32G32B32A32_UINT:
			return GL_UNSIGNED_INT;

		case InputLayoutFormat::R32_SINT:
		case InputLayoutFormat::R32G32_SINT:
		case InputLayoutFormat::R32G32B32_SINT:
		case InputLayoutFormat::R32G32B32A32_SINT:
			return GL_INT;

		case InputLayoutFormat::R16_FLOAT:
		case InputLayoutFormat::R16G16_FLOAT:
		case InputLayoutFormat::R16G16B16A16_FLOAT:
			return GL_HALF_FLOAT;

		case InputLayoutFormat::R16_UINT:
		case InputLayoutFormat::R16G16_UINT:
		case InputLayoutFormat::R16G16B16A16_UINT:
			return GL_UNSIGNED_SHORT;

		case InputLayoutFormat::R16_SINT:
		case InputLayoutFormat::R16G16_SINT:
		case InputLayoutFormat::R16G16B16A16_SINT:
			return GL_SHORT;

		case InputLayoutFormat::R8_UNORM:
		case InputLayoutFormat::R8G8_UNORM:
		case InputLayoutFormat::R8G8B8A8_UNORM:
		case InputLayoutFormat::R8_UINT:
		case InputLayoutFormat::R8G8_UINT:
		case InputLayoutFormat::R8G8B8A8_UINT:
			return GL_UNSIGNED_BYTE;

		case InputLayoutFormat::R8_SINT:
		case InputLayoutFormat::R8G8_SINT:
		case InputLayoutFormat::R8G8B8A8_SINT:
			return GL_BYTE;

		default:
			return GL_INVALID_ENUM;
		}
	}

	inline static bool isNormalizedFormat(InputLayoutFormat fmt)
	{
		switch (fmt)
		{
		case InputLayoutFormat::R8_UNORM:
		case InputLayoutFormat::R8G8_UNORM:
		case InputLayoutFormat::R8G8B8A8_UNORM:
			return true;
		default:
			return false;
		}
	}
}

#endif // !HIPPIOS_GL_INCLUDES_H