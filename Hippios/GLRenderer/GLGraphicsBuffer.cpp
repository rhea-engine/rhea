#include "GLGraphicsBuffer.h"
#include <cstring>

namespace Hippios
{
	GLGraphicsBuffer::GLGraphicsBuffer(GLenum target, size_t size_in_bytes, bool dynamic) :
		m_target(target),
		m_dynamic(dynamic)
	{
		m_size_in_bytes = size_in_bytes;
		glGenBuffers(1, &m_buffer);
		glBindBuffer(m_target, m_buffer);
		GLenum usage = m_dynamic ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW;
		glBufferData(m_target, m_size_in_bytes, nullptr, usage);
		glBindBuffer(m_target, 0);
	}

	GLGraphicsBuffer::~GLGraphicsBuffer()
	{
		if (m_buffer)
			glDeleteBuffers(1, &m_buffer);
	}

	void* GLGraphicsBuffer::Lock(size_t offset, size_t length)
	{
		GraphicsBuffer::Lock(offset, length);
		glBindBuffer(m_target, m_buffer);
		void* ptr = glMapBuffer(m_target, GL_WRITE_ONLY);
		return ptr ? static_cast<char*>(ptr) + offset : nullptr;
	}

	void GLGraphicsBuffer::Unlock()
	{
		glUnmapBuffer(m_target);
		glBindBuffer(m_target, 0);
		GraphicsBuffer::Unlock();
	}

	void GLGraphicsBuffer::ReadData(size_t offset, size_t length, void* dest)
	{
		glBindBuffer(m_target, m_buffer);
		glGetBufferSubData(m_target, offset, length, dest);
		glBindBuffer(m_target, 0);
	}

	void GLGraphicsBuffer::WriteData(size_t offset, size_t length, const void* source, bool discardBuffer, bool mapped)
	{
		glBindBuffer(m_target, m_buffer);
		if (discardBuffer) {
			GLenum usage = m_dynamic ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW;
			glBufferData(m_target, m_size_in_bytes, nullptr, usage);
		}
		glBufferSubData(m_target, offset, length, source);
		glBindBuffer(m_target, 0);
	}

	void GLGraphicsBuffer::update(const void* source)
	{
		glBindBuffer(m_target, m_buffer);
		glBufferSubData(m_target, 0, m_size_in_bytes, source);
		glBindBuffer(m_target, 0);
	}
}