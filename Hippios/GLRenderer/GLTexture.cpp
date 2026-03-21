#include "GLTexture.h"
#include <cstdio>

extern PFNGLGENTEXTURESPROC glGenTextures_; // avoid name collision, or use GLAD
extern PFNGLDELETETEXTURESPROC glDeleteTextures_;
extern PFNGLBINDTEXTUREPROC glBindTexture_;
extern PFNGLTEXIMAGE2DPROC glTexImage2D_;
extern PFNGLTEXSUBIMAGE2DPROC glTexSubImage2D_;
extern PFNGLTEXPARAMETERIPROC glTexParameteri_;

// With GLAD, these are already available as glGenTextures etc.
// For simplicity, use the standard GL names (available via GLAD):

namespace Hippios
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
		glTexParameteri(m_gl_target, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(m_gl_target, GL_TEXTURE_WRAP_T, GL_REPEAT);

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
		glTexSubImage2D(target, 0, 0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, image_data);
		glBindTexture(m_gl_target, 0);
	}

	void GLTexture::loadCubemapFace(int faceIndex, Texture* src_texture)
	{
		// For cubemap: copy pixel data from source texture into this cubemap face
		// A full implementation would use glCopyImageSubData (GL 4.3+) or FBO blit
		GLTexture* source = static_cast<GLTexture*>(src_texture);
		if (!source || !m_texture || !source->m_texture)
			return;

		// TODO: use glCopyImageSubData for GPU-side copy (requires GL 4.3)
		// glCopyImageSubData(source->m_texture, GL_TEXTURE_2D, 0, 0, 0, 0,
		//                    m_texture, GL_TEXTURE_CUBE_MAP, 0, 0, 0, faceIndex,
		//                    m_desc.width, m_desc.height, 1);
	}
}