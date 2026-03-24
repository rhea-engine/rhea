#include "GLRenderer.h"
#include "GLRenderer.h"
#include "GLRenderer.h"
#include "GLVertexBuffer.h"
#include "GLIndexBuffer.h"
#include "GLGraphicsBuffer.h"
#include "GLTexture.h"
#include "GLShader.h"
#include "GLSwapChain.h"
#include "GLRenderTarget.h"
#include "GLPipelineState.h"
#include "GLBlendState.h"
#include "GLSamplerState.h"
#include "GLShaderProgram.h"

#include <cassert>
#include <cstdio>
#include <cstring>
#include <Psio/Log.h>

// WGL extension function pointers (not provided by GLAD)
static PFNWGLCREATECONTEXTATTRIBSARBPROC wglCreateContextAttribsARB = nullptr;

namespace Hippios
{
	GLRenderer::GLRenderer()
	{
	}

	GLRenderer::~GLRenderer()
	{
		if (m_glrc) {
			wglMakeCurrent(nullptr, nullptr);
			wglDeleteContext(m_glrc);
		}
	}

	bool GLRenderer::initOpenGLContext(HWND hwnd)
	{
		m_hdc = GetDC(hwnd);

		// Set pixel format
		PIXELFORMATDESCRIPTOR pfd = {};
		pfd.nSize = sizeof(pfd);
		pfd.nVersion = 1;
		pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
		pfd.iPixelType = PFD_TYPE_RGBA;
		pfd.cColorBits = 32;
		pfd.cDepthBits = 24;
		pfd.cStencilBits = 8;
		pfd.iLayerType = PFD_MAIN_PLANE;

		int format = ChoosePixelFormat(m_hdc, &pfd);
		if (!format) {
			LOG(LOG_ERROR) << "ChoosePixelFormat failed";
			return false;
		}
		SetPixelFormat(m_hdc, format, &pfd);

		// Create legacy context first to load WGL extensions
		HGLRC temp_context = wglCreateContext(m_hdc);
		wglMakeCurrent(m_hdc, temp_context);

		// Load wglCreateContextAttribsARB
		wglCreateContextAttribsARB = (PFNWGLCREATECONTEXTATTRIBSARBPROC)wglGetProcAddress("wglCreateContextAttribsARB");

		if (wglCreateContextAttribsARB) {
			// Create modern OpenGL 4.5 core profile context
			int attribs[] = {
				WGL_CONTEXT_MAJOR_VERSION_ARB, 4,
				WGL_CONTEXT_MINOR_VERSION_ARB, 5,
				WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
#ifdef _DEBUG
				WGL_CONTEXT_FLAGS_ARB, WGL_CONTEXT_DEBUG_BIT_ARB,
#endif
				0
			};

			m_glrc = wglCreateContextAttribsARB(m_hdc, nullptr, attribs);
			wglMakeCurrent(nullptr, nullptr);
			wglDeleteContext(temp_context);

			if (!m_glrc) {
				LOG(LOG_ERROR) << "Failed to create OpenGL 4.5 context";
				return false;
			}
			wglMakeCurrent(m_hdc, m_glrc);
		}
		else {
			m_glrc = temp_context;
		}

		// Initialize GLAD – this loads ALL GL function pointers.
		// wglGetProcAddress only resolves extension and GL >1.1 functions.
		// For core GL 1.1 functions (glGetString, glEnable, …) we must fall
		// back to GetProcAddress on opengl32.dll.  GLAD expects a single
		// loader callback that handles both cases.
		HMODULE opengl_module = GetModuleHandleA("opengl32.dll");
		if (!gladLoadGLLoader([](const char* name) -> void* {
			void* proc = (void*)wglGetProcAddress(name);
			if (!proc || proc == (void*)0x1 || proc == (void*)0x2 ||
				proc == (void*)0x3 || proc == (void*)-1) {
				HMODULE gl = GetModuleHandleA("opengl32.dll");
				proc = (void*)GetProcAddress(gl, name);
			}
			return proc;
		})) {
			LOG(LOG_ERROR) << "Failed to initialize GLAD";
			return false;
		}

		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LESS);

		// Match D3D11 winding order: clockwise = front face
		glFrontFace(GL_CW);
		
		LOG(LOG_INFO) << "OpenGL Renderer: " << glGetString(GL_RENDERER) << ", OpenGL Version: " << glGetString(GL_VERSION);

		return true;
	}

	std::unique_ptr<SwapChain> GLRenderer::createSwapChain(NativeWindow* win)
	{
		if (!m_glrc) {
			if (!initOpenGLContext((HWND)win->native_win_handle))
				return nullptr;
		}
		return std::make_unique<GLSwapChain>(m_hdc);
	}

	std::unique_ptr<RenderTarget> GLRenderer::createRenderTarget(const RenderTargetDesc& desc)
	{
		auto target = std::make_unique<GLRenderTarget>();
		if (target->create(this, desc))
			return target;
		else
			return nullptr;
	}

	std::unique_ptr<VertexBuffer> GLRenderer::createVertexBuffer(size_t vertexSize, size_t numVertices, bool dynamic)
	{
		return std::make_unique<GLVertexBuffer>(vertexSize, numVertices, dynamic);
	}

	std::unique_ptr<IndexBuffer> GLRenderer::createIndexBuffer(size_t vertex_size, size_t num_faces, bool dynamic)
	{
		return std::make_unique<GLIndexBuffer>(vertex_size, num_faces, dynamic);
	}

	std::unique_ptr<GraphicsBuffer> GLRenderer::createGraphicsBuffer(size_t size_in_bytes)
	{
		return std::make_unique<GLGraphicsBuffer>(GL_UNIFORM_BUFFER, size_in_bytes, false);
	}

	std::unique_ptr<Texture> GLRenderer::createTexture(TextureDesc& desc)
	{
		return std::make_unique<GLTexture>(desc);
	}

	ShaderProgram* GLRenderer::createShader(const ShaderProgramDesc& desc)
	{
		auto* program = new GLShaderProgram();
		if (!program->loadFromFiles(desc.vertexFile, desc.vertexEntry, desc.pixelFile, desc.pixelEntry)) {
			delete program;
			return nullptr;
		}
		return program;
	}

	ShaderProgram* GLRenderer::createShader(const ShaderProgramSourceDesc& desc)
	{
		auto* program = new GLShaderProgram();
		program->setSamplerBindings(desc.samplerBindings);
		if (!program->loadFromSource(
			desc.vertexSource, desc.vertexSourceLength, desc.vertexEntry,
			desc.pixelSource, desc.pixelSourceLength, desc.pixelEntry)) {
			delete program;
			return nullptr;
		}
		return program;
	}

	std::unique_ptr<PipelineStateObject> GLRenderer::createPipelineState(const PipelineStateObjectDesc& desc)
	{
		return std::make_unique<GLPipelineStateObject>(desc);
	}

	void GLRenderer::setPipelineState(PipelineStateObject* pso)
	{
		if (m_current_pso != pso) {
			m_current_pso = pso;
			pso->SetState();
		}
		auto* glPso = static_cast<GLPipelineStateObject*>(pso);
		m_current_topology = glPso->getTopology();
	}

	void GLRenderer::setBlendState(BlendState* state, float* blend_factor, unsigned int mask)
	{
		static_cast<GLBlendState*>(state)->setState(blend_factor, mask);
	}

	StencilState* GLRenderer::getStencilState(const std::string& name)
	{
		auto it = m_stencil_states.find(name);
		if (it != m_stencil_states.end())
			return it->second.get();
		return nullptr;
	}

	StencilState* GLRenderer::createStencilState(const std::string& name, const DepthStencilStateDesc& desc)
	{
		auto state = new GLStencilState(desc);
		m_stencil_states[name].reset(state);
		return state;
	}

	void GLRenderer::setStencilState(StencilState* state, int stencil_ref)
	{
		GLStencilState* gl_state = static_cast<GLStencilState*>(state);
		if (gl_state != m_current_stencil_state) {
			m_current_stencil_state = gl_state;
			m_current_stencil_state->setState(stencil_ref);
		}
	}

	SamplerState* GLRenderer::createSampler(const SamplerDesc& desc)
	{
		auto sampler = std::make_unique<GLSamplerState>(desc);
		SamplerState* raw = sampler.get();
		m_sampler_states.push_back(std::move(sampler));
		return raw;
	}

	void GLRenderer::setSampler(int slot, SamplerState* sampler, ShaderStage stage)
	{
		if (sampler) {
			GLuint gl_sampler = static_cast<GLSamplerState*>(sampler)->getSampler();
			// In OpenGL, sampler binding points are shared across all stages
			glBindSampler(slot, gl_sampler);
		}
	}

	void GLRenderer::setTexture(Texture* texture, int slot)
	{
		assert(texture);
		if (!texture) return;

		GLTexture* gl_tex = static_cast<GLTexture*>(texture);

		if (slot == 0 && m_current_texture == texture)
			return;

		if (slot == 0)
			m_current_texture = texture;

		glActiveTexture(GL_TEXTURE0 + slot);
		glBindTexture(gl_tex->getTarget(), gl_tex->get());
	}

	void GLRenderer::bindBuffers(VertexBuffer* vb, IndexBuffer* ib)
	{
		auto* glPso = static_cast<GLPipelineStateObject*>(m_current_pso);
		auto* glVb = static_cast<GLVertexBuffer*>(vb);
		auto* glIb = static_cast<GLIndexBuffer*>(ib);

		if (!glPso)
			return;

		const GLuint vao = glPso->getVAO();
		const InputLayoutDesc& layout = glPso->getInputLayout();

		// Attach the actual vertex buffer(s) to the VAO binding slots.
		// With the current renderer API, we only have one vertex buffer,
		// so this supports layouts where all used input slots map to that one buffer.
		if (glVb && layout.bindings && layout.num_bindings > 0)
		{
			const GLuint buffer = glVb->get_buffer();

			for (int i = 0; i < layout.num_bindings; ++i)
			{
				const InputBindingDesc& binding = layout.bindings[i];

				glVertexArrayVertexBuffer(
					vao,
					static_cast<GLuint>(binding.inputSlot),   // VAO binding slot
					buffer,                                   // actual GL vertex buffer
					0,                                        // base offset in buffer
					static_cast<GLsizei>(binding.stride));    // stride for this slot
			}
		}

		// Attach index buffer to the VAO.
		if (glIb)
		{
			glVertexArrayElementBuffer(vao, glIb->get_buffer());
			m_last_ib = glIb;
			m_current_index_type = glIb->getIndexType();
			m_current_index_size = glIb->getIndexSize();
		}
		else
		{
			// Optional: clear the element buffer if you want indexed and non-indexed
			// draws to be fully explicit.
			glVertexArrayElementBuffer(vao, 0);
			m_last_ib = nullptr;
			m_current_index_type = GL_UNSIGNED_INT;
			m_current_index_size = sizeof(uint32_t);
		}
	}

	void GLRenderer::clearPass(RenderTarget* render_target)
	{
		render_target->clearPass();
	}

	int GLRenderer::presentPass(SwapChain* swap_chain)
	{
		if (!swap_chain)
			return 0;

		GLSwapChain* gl_swap = static_cast<GLSwapChain*>(swap_chain);
		if (!SwapBuffers(gl_swap->getHDC())) {
			LOG(LOG_ERROR) << "SwapBuffers failed";
			return -1;
		}
		return 0;
	}

	bool GLRenderer::getStencilStateDesc(const std::string& name, DepthStencilStateDesc& outDesc)
	{
		GLStencilState* state = static_cast<GLStencilState*>(getStencilState(name));
		if (state) {
			outDesc = state->getDesc();
			return true;
		}
		return false;
	}

	void GLRenderer::draw(int size, int start_vtx)
	{
		glDrawArrays(m_current_topology, start_vtx, size);
	}

	void GLRenderer::drawIndexed(int size, int start_idx, int start_vtx)
	{
		glDrawElementsBaseVertex(
			m_current_topology,
			size,
			m_current_index_type,
			reinterpret_cast<void*>(static_cast<uintptr_t>(start_idx * m_current_index_size)),
			start_vtx);
	}

	GraphicsBuffer* GLRenderer::initGPUBuffer(size_t size)
	{
		// UBOs should be aligned to 16 bytes (like D3D11 constant buffers)
		if (size % 16) {
			size_t base = size / 16;
			size = (base + 1) * 16;
		}
		return new GLGraphicsBuffer(GL_UNIFORM_BUFFER, size, false);
	}

	void GLRenderer::bindGPUBuffer(GraphicsBuffer* buffer, int slot, ShaderStage stage)
	{
		GLGraphicsBuffer* gl_buf = static_cast<GLGraphicsBuffer*>(buffer);
		// In OpenGL, uniform buffer binding points are shared across all stages
		glBindBufferBase(GL_UNIFORM_BUFFER, slot, gl_buf->get_buffer());
	}

	void GLRenderer::prepareMatricesForShader(Math::Matrix4* dst, const Math::Matrix4* src, size_t count) const
	{
		for (size_t i = 0; i < count; ++i)
			dst[i] = src[i].transpose();
	}
}