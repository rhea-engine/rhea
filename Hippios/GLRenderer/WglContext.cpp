#include "WglContext.h"

#include <Psio/Log.h>
#include <GLSwapChainWin32.h>

namespace
{
	// WGL extension function pointer (not provided by GLAD headers directly)
	PFNWGLCREATECONTEXTATTRIBSARBPROC wglCreateContextAttribsARB = nullptr;

	// Initialize GLAD – this loads ALL GL function pointers.
	// wglGetProcAddress only resolves extension and GL >1.1 functions.
	// For core GL 1.1 functions (glGetString, glEnable, …) we must fall
	// back to GetProcAddress on opengl32.dll.  GLAD expects a single
	// loader callback that handles both cases.
	void* loadOpenGLProc(const char* name)
	{
		void* proc = reinterpret_cast<void*>(wglGetProcAddress(name));
		if (!proc || proc == reinterpret_cast<void*>(0x1) ||
			proc == reinterpret_cast<void*>(0x2) ||
			proc == reinterpret_cast<void*>(0x3) ||
			proc == reinterpret_cast<void*>(-1))
		{
			HMODULE module = ::GetModuleHandleA("opengl32.dll");
			if (!module)
				return nullptr;

			proc = reinterpret_cast<void*>(::GetProcAddress(module, name));
		}
		return proc;

		/*
		if (!gladLoadGLLoader((GLADloadproc)wglGetProcAddress)) {
			LOG(LOG_ERROR) << "Failed to initialize GLAD";
			wglMakeCurrent(nullptr, nullptr);
			wglDeleteContext(m_glrc);
			m_glrc = nullptr;
			return false;
		}
		*/
	}
}

namespace Hippios
{
	WglContext::WglContext()
	{
	}

	WglContext::~WglContext()
	{
		destroy();
	}

	bool WglContext::prepareSwapChain(SwapChain* swapChain)
	{
		auto* sc = static_cast<GLSwapChainWin32*>(swapChain);

		HDC hdc = sc->getHDC();
		if (!hdc)
		{
			LOG(LOG_ERROR) << "WglContext::prepareSwapChain: invalid HDC";
			return false;
		}

		return setupPixelFormat(hdc);
	}

	bool WglContext::create(SwapChain* swapChain)
	{
		if (m_glrc || !swapChain)
			return true;

		auto* sc = static_cast<GLSwapChainWin32*>(swapChain);

		HDC hdc = sc->getHDC();
		if (!hdc)
		{
			LOG(LOG_ERROR) << "WglContext::create: invalid HDC";
			return false;
		}

		if (!createSharedContext(hdc, nullptr)) {
			LOG(LOG_ERROR) << "WglContext::create: createSharedContext failed";
			return false;
		}

		return true;
	}
	void WglContext::destroy()
	{
		if (m_glrc)
		{
			::wglMakeCurrent(nullptr, nullptr);
			::wglDeleteContext(m_glrc);
			m_glrc = nullptr;
		}
	}

	bool WglContext::isCreated() const
	{
		return m_glrc != nullptr;
	}

	bool WglContext::makeCurrent(SwapChain* swap_chain)
	{
		if (!swap_chain || !m_glrc)
			return false;

		GLSwapChainWin32* gl_swap = static_cast<GLSwapChainWin32*>(swap_chain);
		HDC hdc = gl_swap->getHDC();
		if (!hdc)
		{
			LOG(LOG_ERROR) << "WglContext::makeCurrent: invalid HDC";
			return false;
		}

		if (!wglMakeCurrent(hdc, m_glrc)) {
			LOG(LOG_ERROR) << "WglContext::makeCurrent: wglMakeCurrent failed";
			return false;
		}

		return true;
	}

	void WglContext::doneCurrent()
	{
		::wglMakeCurrent(nullptr, nullptr);
	}

	void WglContext::swapBuffers(SwapChain* swap_chain)
	{
		if (!swap_chain || !m_glrc)
			return;

		GLSwapChainWin32* gl_swap = static_cast<GLSwapChainWin32*>(swap_chain);
		HDC hdc = gl_swap->getHDC();
		if (!hdc)
		{
			LOG(LOG_ERROR) << "WglContext::swapBuffers: invalid HDC";
			return;
		}

		if (!SwapBuffers(hdc)) {
			LOG(LOG_ERROR) << "WglContext::swapBuffers: SwapBuffers failed";
			return;
		}
	}

	bool WglContext::setupPixelFormat(HDC hdc)
	{
		if (!hdc)
			return false;

		// SetPixelFormat may only be called once for a window DC.
		// If a format is already present, accept it as success for now.
		const int existingFormat = ::GetPixelFormat(hdc);
		if (existingFormat != 0)
			return true;

		PIXELFORMATDESCRIPTOR pfd = {};
		pfd.nSize = sizeof(pfd);
		pfd.nVersion = 1;
		pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
		pfd.iPixelType = PFD_TYPE_RGBA;
		pfd.cColorBits = 32;
		pfd.cDepthBits = 24;
		pfd.cStencilBits = 8;
		pfd.iLayerType = PFD_MAIN_PLANE;

		int format = ChoosePixelFormat(hdc, &pfd);
		if (!format) {
			LOG(LOG_ERROR) << "WglContext::setupPixelFormat: ChoosePixelFormat failed";
			return false;
		}

		if (!SetPixelFormat(hdc, format, &pfd)) {
			LOG(LOG_ERROR) << "WglContext::setupPixelFormat: SetPixelFormat failed";
			return false;
		}

		return true;
	}

	bool WglContext::createSharedContext(HDC hdc, HGLRC sharedContext)
	{
		if (!hdc)
			return false;

		// Create legacy context first so WGL extension loading works.
		HGLRC temp_context = wglCreateContext(hdc);
		if (!temp_context) {
			LOG(LOG_ERROR) << "WglContext::createSharedContext: wglCreateContext failed";
			return false;
		}

		if (!wglMakeCurrent(hdc, temp_context)) {
			LOG(LOG_ERROR) << "WglContext::createSharedContext: wglMakeCurrent(temp_context) failed";
			wglDeleteContext(temp_context);
			return false;
		}

		if (sharedContext)
		{
			if (!::wglShareLists(sharedContext, temp_context))
			{
				LOG(LOG_ERROR) << "WglContext::createSharedContext: wglShareLists failed";
				::wglMakeCurrent(nullptr, nullptr);
				::wglDeleteContext(temp_context);
				return false;
			}
		}

		// Load wglCreateContextAttribsARB
		wglCreateContextAttribsARB = (PFNWGLCREATECONTEXTATTRIBSARBPROC)wglGetProcAddress("wglCreateContextAttribsARB");

		if (wglCreateContextAttribsARB) {
			int attribs[] = {
				WGL_CONTEXT_MAJOR_VERSION_ARB, 4,
				WGL_CONTEXT_MINOR_VERSION_ARB, 5,
				WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
	#ifdef _DEBUG
				WGL_CONTEXT_FLAGS_ARB, WGL_CONTEXT_DEBUG_BIT_ARB,
	#endif
				0
			};

			m_glrc = wglCreateContextAttribsARB(hdc, nullptr, attribs);

			wglMakeCurrent(nullptr, nullptr);
			wglDeleteContext(temp_context);
			if (!m_glrc) {
				LOG(LOG_ERROR) << "WglContext::createSharedContext: Failed to create OpenGL 4.5 context";
				return false;
			}

			if (!wglMakeCurrent(hdc, m_glrc)) {
				LOG(LOG_ERROR) << "WglContext::createSharedContext: wglMakeCurrent(shared context) failed";
				wglDeleteContext(m_glrc);
				m_glrc = nullptr;
				return false;
			}
		}
		else {
			m_glrc = temp_context;
		}

		if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(loadOpenGLProc)))
		{
			LOG(LOG_ERROR) << "WglContext::createSharedContext: failed to initialize GLAD";
			::wglMakeCurrent(nullptr, nullptr);
			::wglDeleteContext(m_glrc);
			m_glrc = nullptr;
			return false;
		}

		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LESS);

		// Match D3D11 winding order: clockwise = front face
		glFrontFace(GL_CW);

		LOG(LOG_INFO) << "OpenGL: " << glGetString(GL_RENDERER) << ", Version: " << glGetString(GL_VERSION);

		return true;
	}
}