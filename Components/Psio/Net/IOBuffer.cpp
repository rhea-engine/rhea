#include "IOBuffer.h"
#include "IOBuffer.h"

#include <malloc.h>
#include <string.h>

namespace Psio
{
	IOBuffer::IOBuffer(size_t size)
	{
		m_buffer = (char*)malloc(size);
		m_size = size;
		m_offset = 0;
		m_write_offset = 0;
	}

	IOBuffer::~IOBuffer()
	{
		if (m_buffer)
			free(m_buffer);
	}

	void IOBuffer::Write(const char* chunk, size_t size)
	{
		if (m_size < m_write_offset + size)
			Grow(size);
		memcpy(m_buffer + m_write_offset, chunk, size);
		m_write_offset += size;
	}

	void IOBuffer::Grow(size_t size)
	{
		char* newbuf;
		if (!m_buffer)
			newbuf = (char*)malloc(size);
		else
			newbuf = (char*)realloc(m_buffer, m_size + size);
		if (newbuf) {
			m_size += size;
			m_buffer = newbuf;
		}
	}

	void IOBuffer::Consume(size_t bytes)
	{
		m_offset += bytes; 
		if (m_offset == m_write_offset) {
			m_offset = 0;
			m_write_offset = 0;
		}
	}
}