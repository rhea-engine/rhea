/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#include "GLRenderer.h"
#include "GLRenderer.h"
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
#include "GLContext.h"

#include <cassert>
#include <cstdio>
#include <cstring>
#include <Psio/Log.h>
#ifdef _WIN32
#include "GLSwapChainWin32.h"
#include "WglContext.h"
#elif defined(__linux__)
//#include "GlxContext.h"
#endif
#include "GLSwapChainSDL.h"
#include "SdlContext.h"

namespace hippios
{
	static const InputBindingDesc* findBindingDesc(const InputLayoutDesc& layout, int inputSlot)
	{
		if (!layout.bindings)
			return nullptr;

		for (int i = 0; i < layout.num_bindings; ++i)
		{
			if (layout.bindings[i].inputSlot == inputSlot)
				return &layout.bindings[i];
		}
		return nullptr;
	}

	GLRenderer::GLRenderer(WindowBackend window_backend) :
		m_backend(window_backend)
	{
		switch (window_backend)
		{
#ifdef _WIN32
		case WindowBackend::Win32:
			m_context = std::make_unique<WglContext>(this);
			break;
#endif // _WIN32
		case WindowBackend::SDL:
			m_context = std::make_unique<SdlGlContext>(this);
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
#if HIPPIOS_HAS_WIN32
		case WindowBackend::Win32:
			swapChain = std::make_unique<GLSwapChainWin32>(win);
			break;
#endif // HIPPIOS_HAS_WIN32
#if HIPPIOS_HAS_SDL
		case WindowBackend::SDL:
			swapChain = std::make_unique<GLSwapChainSDL>(win);
			break;
#endif // HIPPIOS_SDL
		case WindowBackend::X11:
		case WindowBackend::Wayland:
		case WindowBackend::GLFW:
		default:
			// Support for these backends not implemented yet
			LOG(LOG_ERROR) << "SwapChain creation not supported for backend: " << static_cast<int>(m_backend);
			throw std::runtime_error("SwapChain creation not supported for this backend");
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
			if (!m_context->create(swapChain.get())) {
				throw std::runtime_error("Failed to create GL context for swap chain");
				return nullptr;
			}
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
		return std::make_unique<GLPipelineStateObject>(m_caps, desc);
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

	bool GLRenderer::ensureContext()
	{
		if (m_context_ready) {
			return true;
		}
		
		if (!gladLoadGL()) {
			LOG(LOG_ERROR) << "No OpenGL context available";
			return false;
		}

		m_context_ready = true;
		return true;
	}

	SamplerState* GLRenderer::createSampler(const SamplerDesc& desc)
	{
		if (!ensureContext()) {
			LOG(LOG_ERROR) << "Cannot create sampler: No OpenGL context available";
			return nullptr;
		}

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
		if (!texture)
		{
			// Unbind the texture slot
			glActiveTexture(GL_TEXTURE0 + slot);
			glBindTexture(GL_TEXTURE_2D, 0);
			if (slot == 0)
				m_current_texture = nullptr;
			return;
		}

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
		
		// Bind VAO first (required for both DSA and non-DSA paths)
		glBindVertexArray(vao);

		if (m_caps.hasDSA)
		{
			// Modern DSA path

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
		else {
			// Legacy non-DSA path

			if (layout.num_bindings > 1)
			{
				LOG(LOG_ERROR) << "OpenGL renderer currently supports only one vertex buffer binding";
				return;
			}

			if (glVb)
			{
				const GLuint buffer = glVb->get_buffer();

				// In legacy GL, the VAO stores attribute format + the ARRAY_BUFFER bound
				// at glVertexAttribPointer/IPointer time, so we must re-specify attributes here.
				glBindBuffer(GL_ARRAY_BUFFER, buffer);

				if (layout.elements && layout.num_elements > 0)
				{
					for (int i = 0; i < layout.num_elements; ++i)
					{
						const auto& elem = layout.elements[i];

						const InputBindingDesc* binding = findBindingDesc(layout, elem.input_slot);
						if (!binding)
							continue;

						const GLuint attribIndex = static_cast<GLuint>(elem.location);
						const GLint components = getFormatComponentCount(elem.format);
						const GLenum glType = getFormatType(elem.format);
						const GLsizei stride = static_cast<GLsizei>(binding->stride);
						const void* offset = reinterpret_cast<const void*>(
							static_cast<uintptr_t>(elem.aligned_byte_offset));

						glEnableVertexAttribArray(attribIndex);

						if (isIntegerFormat(elem.format))
						{
							glVertexAttribIPointer(
								attribIndex,
								components,
								glType,
								stride,
								offset);
						}
						else
						{
							glVertexAttribPointer(
								attribIndex,
								components,
								glType,
								isNormalizedFormat(elem.format) ? GL_TRUE : GL_FALSE,
								stride,
								offset);
						}

						glVertexAttribDivisor(
							attribIndex,
							static_cast<GLuint>(binding->instanceStepRate));
					}
				}
			}

			if (glIb)
			{
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, glIb->get_buffer());
				m_last_ib = glIb;
				m_current_index_type = glIb->getIndexType();
				m_current_index_size = glIb->getIndexSize();
			}
			else
			{
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
				m_last_ib = nullptr;
				m_current_index_type = GL_UNSIGNED_INT;
				m_current_index_size = sizeof(uint32_t);
			}
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

	void GLRenderer::prepareMatricesForShader(math::Matrix4* dst, const math::Matrix4* src, size_t count) const
	{
		for (size_t i = 0; i < count; ++i)
			dst[i] = src[i].transpose();
	}

	void hippios::GLRenderer::detectCapabilities()
	{
		m_caps.majorVersion = GLVersion.major;
		m_caps.minorVersion = GLVersion.minor;

	//	m_caps.hasDSA = (GLAD_GL_VERSION_4_5 != 0);
		m_caps.hasDebugOutput = (GLAD_GL_VERSION_4_3 != 0);
		m_caps.hasDSA = false;

		LOG(LOG_INFO) << "Detected OpenGL version " << m_caps.majorVersion << "." << m_caps.minorVersion
			<< " - DSA: " << (m_caps.hasDSA ? "Yes" : "No")
			<< ", Debug Output: " << (m_caps.hasDebugOutput ? "Yes" : "No");		
	}
}