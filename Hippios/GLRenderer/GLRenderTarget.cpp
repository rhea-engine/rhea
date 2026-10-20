/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#include "GLRenderTarget.h"
#include "GLRenderer.h"
#include <cstdio>

namespace hippios
{
	GLRenderTarget::GLRenderTarget() :
		m_framebuffer(0),
		m_color_texture(0),
		m_is_default(true)
	{
	}

	GLRenderTarget::~GLRenderTarget()
	{
		Release();
	}

	bool GLRenderTarget::create(Renderer* renderer, const RenderTargetDesc& desc)
	{
		m_renderer = renderer;
		m_width = desc.width;
		m_height = desc.height;

		Release();

		if (desc.type == RenderTargetType::SwapChain)
		{
			// Default framebuffer (0) — backbuffer from the swap chain.
			// Depth/stencil is provided by the WGL pixel format, no renderbuffer needed.
			m_framebuffer = 0;
			m_is_default = true;
			m_swap_chain = desc.swap_chain;
		}
		else
		{
			// Offscreen FBO with color texture attachment
			m_is_default = false;
			m_swap_chain = nullptr;

			glGenFramebuffers(1, &m_framebuffer);
			glBindFramebuffer(GL_FRAMEBUFFER, m_framebuffer);

			// Create color texture
			glGenTextures(1, &m_color_texture);
			glBindTexture(GL_TEXTURE_2D, m_color_texture);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8,
				desc.width, desc.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
				GL_TEXTURE_2D, m_color_texture, 0);

			// Create depth renderbuffer only for offscreen FBOs
			if (desc.depth_bits > 0)
			{
				m_depth_buffer = std::make_unique<GLDepthBuffer>(desc.depth_bits, desc.width, desc.height);
				glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT,
					GL_RENDERBUFFER, m_depth_buffer->getRenderbuffer());
			}

			GLenum drawBuffer = GL_COLOR_ATTACHMENT0;
			glDrawBuffers(1, &drawBuffer);

			GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
			if (status != GL_FRAMEBUFFER_COMPLETE) {
				printf("GLRenderTarget: Framebuffer incomplete: 0x%X\n", status);
				glBindFramebuffer(GL_FRAMEBUFFER, 0);
				return false;
			}

			glBindFramebuffer(GL_FRAMEBUFFER, 0);
		}

		return true;
	}

	bool GLRenderTarget::resize(int width, int height)
	{
		if (m_width == width && m_height == height)
			return true;

		m_width = width;
		m_height = height;

		if (m_is_default)
		{
			// Default framebuffer resizes automatically with the window.
			// Nothing to do here.
			return true;
		}

		// Resize offscreen color texture
		if (m_color_texture) {
			glBindTexture(GL_TEXTURE_2D, m_color_texture);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8,
				width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
			glBindTexture(GL_TEXTURE_2D, 0);
		}

		// Recreate depth buffer with new dimensions
		if (m_depth_buffer) {
			m_depth_buffer = std::make_unique<GLDepthBuffer>(24, width, height);
			glBindFramebuffer(GL_FRAMEBUFFER, m_framebuffer);
			glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT,
				GL_RENDERBUFFER, m_depth_buffer->getRenderbuffer());
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
		}

		return true;
	}

	void GLRenderTarget::Release()
	{
		if (!m_is_default && m_framebuffer) {
			glDeleteFramebuffers(1, &m_framebuffer);
			m_framebuffer = 0;
		}
		if (m_color_texture) {
			glDeleteTextures(1, &m_color_texture);
			m_color_texture = 0;
		}
		m_depth_buffer.reset();
	}

	void GLRenderTarget::setActive()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, m_framebuffer);
		static_cast<GLRenderer*>(m_renderer)->setRenderTargetHeight(m_height);
	}

	void GLRenderTarget::clearPass()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, m_framebuffer);

		// Clear the entire target independently of the previous GUI draw state.
		const GLboolean scissor_enabled = glIsEnabled(GL_SCISSOR_TEST);
		GLboolean depth_mask;
		GLboolean color_mask[4];
		GLint stencil_front_mask;
		GLint stencil_back_mask;
		glGetBooleanv(GL_DEPTH_WRITEMASK, &depth_mask);
		glGetBooleanv(GL_COLOR_WRITEMASK, color_mask);
		glGetIntegerv(GL_STENCIL_WRITEMASK, &stencil_front_mask);
		glGetIntegerv(GL_STENCIL_BACK_WRITEMASK, &stencil_back_mask);
		glDisable(GL_SCISSOR_TEST);
		glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
		glStencilMask(~GLuint(0));

		// Ensure depth writes are enabled — glClear respects glDepthMask.
		// Without this, if the previous frame's last PSO had depth writes
		// disabled (e.g. skybox with DEPTH_WRITE_ZERO), the depth buffer
		// will NOT be cleared, causing stale depth values and z-fighting.
		glDepthMask(GL_TRUE);

		glClearColor(0.0f, 0.0f, 0.5f, 0.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
		// Preserve state expected by the pipeline-state cache.
		glDepthMask(depth_mask);
		glColorMask(color_mask[0], color_mask[1], color_mask[2], color_mask[3]);
		glStencilMaskSeparate(GL_FRONT, static_cast<GLuint>(stencil_front_mask));
		glStencilMaskSeparate(GL_BACK, static_cast<GLuint>(stencil_back_mask));
		if (scissor_enabled)
		    glEnable(GL_SCISSOR_TEST);
	}

	void GLRenderTarget::setViewport(ViewportDesc* desc)
	{
		glViewport(
			static_cast<GLint>(desc->x),
			static_cast<GLint>(desc->y),
			static_cast<GLsizei>(desc->w),
			static_cast<GLsizei>(desc->h));
		glDepthRange(desc->min_depth, desc->max_depth);
	}
}