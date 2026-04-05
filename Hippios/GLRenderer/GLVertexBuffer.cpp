#include "GLVertexBuffer.h"

namespace hippios
{
	GLVertexBuffer::GLVertexBuffer(size_t vertex_size, size_t num_vertices, bool dynamic) :
		is_dirty(true)
	{
		m_vertex_size = vertex_size;
		m_num_vertices = num_vertices;
		m_size_in_bytes = vertex_size * num_vertices;
		m_buffer = std::make_unique<GLGraphicsBuffer>(GL_ARRAY_BUFFER, m_size_in_bytes, dynamic);
	}

	void* GLVertexBuffer::Lock(size_t offset, size_t length)
	{
		return m_buffer->Lock(offset, length);
	}

	void GLVertexBuffer::Unlock()
	{
		m_buffer->Unlock();
	}

	void GLVertexBuffer::ReadData(size_t offset, size_t length, void* dest)
	{
		m_buffer->ReadData(offset, length, dest);
	}

	void GLVertexBuffer::WriteData(size_t offset, size_t length, const void* source, bool discardBuffer, bool mapped)
	{
		m_buffer->WriteData(offset, length, source, discardBuffer, mapped);
		is_dirty = true;
	}

	void GLVertexBuffer::update(const void* source)
	{
		m_buffer->update(source);
		is_dirty = true;
	}
}