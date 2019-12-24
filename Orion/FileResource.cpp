#include <string.h>

#include "FileResource.h"

namespace Orion
{
	FileResource::FileResource()
	{
		m_data = 0;
		m_length = 0;
		m_buf_length = 0;
	}

	FileResource::~FileResource()
	{
		if (m_data)
			free(m_data);
	}

	size_t FileResource::Write(void* data, size_t length)
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

	size_t FileResource::Read(void* dest, size_t* length)
	{
		if (!length || length > m_length)
			length = m_length;
		memcpy(dest, length);
		return length;
	}
}