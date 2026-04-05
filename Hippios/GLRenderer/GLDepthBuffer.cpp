#include "GLDepthBuffer.h"

namespace hippios
{
	GLDepthBuffer::GLDepthBuffer(unsigned int bit_depth, unsigned int width, unsigned int height) :
		DepthBuffer(bit_depth, width, height)
	{
		GLenum format = GL_DEPTH24_STENCIL8;
		if (bit_depth == 32)
			format = GL_DEPTH32F_STENCIL8;

		glGenRenderbuffers(1, &m_renderbuffer);
		glBindRenderbuffer(GL_RENDERBUFFER, m_renderbuffer);
		glRenderbufferStorage(GL_RENDERBUFFER, format, width, height);
		glBindRenderbuffer(GL_RENDERBUFFER, 0);
	}

	GLDepthBuffer::~GLDepthBuffer()
	{
		if (m_renderbuffer)
			glDeleteRenderbuffers(1, &m_renderbuffer);
	}
}