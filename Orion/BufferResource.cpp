#include <string.h>

#include "BufferResource.h"

namespace Orion
{
	BufferResource::BufferResource()
	{		
		m_data = 0;
		m_length = 0;
		m_buf_length = 0;
	}

	BufferResource::~BufferResource()
	{
		if (m_data)
			free(m_data);
	}

	size_t BufferResource::Write(void* data, size_t length)
	{
		if (m_data && m_buf_length < length) {
			m_data = realloc(m_data, length);
			m_buf_length = length;
		}
		else if (!m_data) {
			m_data = malloc(length);
			m_buf_length = length;
		}
		memcpy(m_data, data, length);
		m_length = length;

		return length;
	}

	size_t BufferResource::Read(void* dest, size_t length)
	{
		if (!length || length > m_length)
			length = m_length;
		memcpy(dest, m_data, length);
		return length;
	}
}