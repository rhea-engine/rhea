#include "GLIndexBuffer.h"

namespace hippios
{
	GLIndexBuffer::GLIndexBuffer(size_t index_stride, size_t num_faces, bool dynamic) :
		is_dirty(true)
	{
		m_index_stride = index_stride;
		m_num_faces = num_faces;
		m_size_in_bytes = index_stride * num_faces * 3;
		if (index_stride == 2)
			m_index_type = GL_UNSIGNED_SHORT;
		else
			m_index_type = GL_UNSIGNED_INT;
		m_buffer = std::make_unique<GLGraphicsBuffer>(GL_ELEMENT_ARRAY_BUFFER, m_size_in_bytes, dynamic);
	}

	void* GLIndexBuffer::Lock(size_t offset, size_t length)
	{
		return m_buffer->Lock(offset, length);
	}

	void GLIndexBuffer::Unlock()
	{
		m_buffer->Unlock();
	}

	void GLIndexBuffer::ReadData(size_t offset, size_t length, void* dest)
	{
		m_buffer->ReadData(offset, length, dest);
	}

	void GLIndexBuffer::WriteData(size_t offset, size_t length, const void* source, bool discardBuffer, bool mapped)
	{
		m_buffer->WriteData(offset, length, source, discardBuffer, mapped);
		is_dirty = true;
	}

	void GLIndexBuffer::update(const void* source)
	{
		m_buffer->update(source);
		is_dirty = true;
	}
}