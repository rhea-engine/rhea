/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#include "GLTexture.h"
#include <cstdio>
#include <Psio/Log.h>
#include <vector>

extern PFNGLGENTEXTURESPROC glGenTextures_; // avoid name collision, or use GLAD
extern PFNGLDELETETEXTURESPROC glDeleteTextures_;
extern PFNGLBINDTEXTUREPROC glBindTexture_;
extern PFNGLTEXIMAGE2DPROC glTexImage2D_;
extern PFNGLTEXSUBIMAGE2DPROC glTexSubImage2D_;
extern PFNGLTEXPARAMETERIPROC glTexParameteri_;

// With GLAD, these are already available as glGenTextures etc.
// For simplicity, use the standard GL names (available via GLAD):

namespace hippios
{
	GLTexture::GLTexture(TextureDesc& desc) :
		Texture(desc)
	{
		if (desc.type == TextureDesc::TextureType::TEXTURE_CUBE ||
			desc.type == TextureDesc::TextureType::TEXTURE_CUBE_ARRAY)
			m_gl_target = GL_TEXTURE_CUBE_MAP;
		else
			m_gl_target = GL_TEXTURE_2D;

		glGenTextures(1, &m_texture);
		glBindTexture(m_gl_target, m_texture);

		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

		if (m_gl_target == GL_TEXTURE_CUBE_MAP) {
			for (int i = 0; i < 6; ++i) {
				glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA8,
					desc.width, desc.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
			}
		}
		else {
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8,
				desc.width, desc.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
		}
		
		glTexParameteri(m_gl_target, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(m_gl_target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		if (m_gl_target == GL_TEXTURE_CUBE_MAP) {
			glTexParameteri(m_gl_target, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(m_gl_target, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTexParameteri(m_gl_target, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
		}
		else {
			glTexParameteri(m_gl_target, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(m_gl_target, GL_TEXTURE_WRAP_T, GL_REPEAT);
		}

		glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
		glBindTexture(m_gl_target, 0);
	}

	GLTexture::~GLTexture()
	{
		if (m_texture)
			glDeleteTextures(1, &m_texture);
	}

	void GLTexture::loadImpl(int index, unsigned int width, unsigned int height, unsigned char* image_data)
	{
		if (!width || !height || !image_data || !m_texture)
			return;

		GLenum target = m_gl_target;
		if (m_gl_target == GL_TEXTURE_CUBE_MAP)
			target = GL_TEXTURE_CUBE_MAP_POSITIVE_X + index;

		glBindTexture(m_gl_target, m_texture);
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		glTexSubImage2D(target, 0, 0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, image_data);
		glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
		glBindTexture(m_gl_target, 0);
	}

	void GLTexture::loadCubemapFace(int faceIndex, Texture* src_texture)
	{
		GLTexture* source = static_cast<GLTexture*>(src_texture);
		if (!source || !m_texture || !source->m_texture)
			return;

		if (faceIndex < 0 || faceIndex >= 6) {
			LOG(LOG_ERROR) << "GLTexture::loadCubemapFace: invalid face index " << faceIndex;
			return;
		}

		// Read pixel data from the source 2D texture and upload into the cubemap face.
		// This avoids compatibility issues with glCopyImageSubData across different
		// internal-format combinations and driver quirks.
		GLsizei w = static_cast<GLsizei>(m_desc.width);
		GLsizei h = static_cast<GLsizei>(m_desc.height);

		std::vector<unsigned char> pixels(static_cast<size_t>(w) * h * 4);

		// Read from source texture via FBO
		GLuint fbo = 0;
		glGenFramebuffers(1, &fbo);
		glBindFramebuffer(GL_FRAMEBUFFER, fbo);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, source->m_texture, 0);

		GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
		if (status != GL_FRAMEBUFFER_COMPLETE) {
			LOG(LOG_ERROR) << "GLTexture::loadCubemapFace: framebuffer incomplete (status 0x"
				<< std::hex << status << std::dec << ")";
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			glDeleteFramebuffers(1, &fbo);
			return;
		}

		glReadPixels(0, 0, w, h, GL_RGBA, GL_UNSIGNED_BYTE, pixels.data());

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glDeleteFramebuffers(1, &fbo);

		// Upload into the cubemap face
		glBindTexture(GL_TEXTURE_CUBE_MAP, m_texture);
		glTexSubImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + faceIndex,
			0, 0, 0, w, h, GL_RGBA, GL_UNSIGNED_BYTE, pixels.data());
		glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
	}
}