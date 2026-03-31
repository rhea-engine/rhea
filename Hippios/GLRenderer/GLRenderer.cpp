#include "GLRenderer.h"
#include "GLVertexBuffer.h"
#include "GLIndexBuffer.h"
#include "GLGraphicsBuffer.h"
#include "GLTexture.h"
#include "GLShader.h"
#include "GLRenderTarget.h"
#include "GLPipelineState.h"
#include "GLBlendState.h"
#include "GLSamplerState.h"
#include "GLShaderProgram.h"
#include "GlContext.h"

#include <cassert>
#include <cstdio>
#include <cstring>
#include <Psio/Log.h>
#ifdef _WIN32
#include "GLSwapChainWin32.h"
#include "WglContext.h"
#elif defined(__linux__)
#include "GlxContext.h"
#endif

namespace Hippios
{
	GLRenderer::GLRenderer(WindowBackend window_backend) :
		m_backend(window_backend)
	{
		switch (window_backend)
		{
#ifdef _WIN32
		case WindowBackend::Win32:
			m_context = std::make_unique<WglContext>();
			break;
#endif // _WIN32
		case WindowBackend::SDL:
			// SDL support not implemented yet
			LOG(LOG_ERROR) << "SDL backend not supported yet in GLRenderer.";
			break;
#ifdef __linux__
		case WindowBackend::X11:
			//m_context = std::make_unique<GlxContext>();
			break;

		case WindowBackend::Wayland:
			// Wayland support not implemented yet
			LOG(LOG_ERROR) << "Wayland backend not supported yet in GLRenderer.";
			break;

		case WindowBackend::GLFW:
			// GLFW support not implemented yet
			LOG(LOG_ERROR) << "GLFW backend not supported yet in GLRenderer.";
			break;

#endif // __linux__
		default:
			LOG(LOG_ERROR) << "Unsupported window backend for GLRenderer: " << static_cast<int>(window_backend);
			break;

		}
	}

	GLRenderer::~GLRenderer() = default;

	std::unique_ptr<SwapChain> GLRenderer::createSwapChain(NativeWindow* win)
	{
		if (!win || !win->native_win_handle || !m_context)
			return nullptr;

		std::unique_ptr<SwapChain> swapChain;

		switch (m_backend)
		{
		case WindowBackend::Win32:
			swapChain = std::make_unique<GLSwapChainWin32>(win);
			break;
		case WindowBackend::SDL:
		case WindowBackend::X11:
		case WindowBackend::Wayland:
		case WindowBackend::GLFW:
			// Support for these backends not implemented yet
			LOG(LOG_ERROR) << "SwapChain creation not supported for backend: " << static_cast<int>(m_backend);
			return nullptr;
		}

		if (!swapChain || !swapChain->isValid())
			return nullptr;

		// Every window/DC must get a valid pixel format
		if (!m_context->prepareSwapChain(swapChain.get()))
			return nullptr;

		// Shared GL context only created once
		if (!m_context->isCreated())
		{
			if (!m_context->create(swapChain.get()))
				return nullptr;
		}

		return swapChain;
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

		m_last_vb = glVb;

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

	bool GLRenderer::setActiveSwapChain(SwapChain* swap_chain)
	{
		if (!swap_chain)
			return false;

		return m_context->makeCurrent(swap_chain);

		
	}

	void GLRenderer::clearPass(RenderTarget* render_target)
	{
		if (!render_target)
			return;
		render_target->clearPass();
	}

	int GLRenderer::presentPass(SwapChain* swap_chain)
	{
		if (!m_context || !swap_chain)
			return -1;

		m_context->swapBuffers(swap_chain);
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