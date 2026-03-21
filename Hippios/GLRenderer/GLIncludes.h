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

#endif // !HIPPIOS_GL_INCLUDES_H