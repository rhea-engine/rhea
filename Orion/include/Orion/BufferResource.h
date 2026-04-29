/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#ifndef ORION_BUFFERRESOURCE_H
#define ORION_BUFFERRESOURCE_H

#include <string>

#include <Orion/Resource.h>

using std::string;

namespace orion
{
	class BufferResource : public Resource
	{
	protected:		
		void* m_data;
		size_t m_length;
		size_t m_buf_length;
	public:
		BufferResource();
		~BufferResource();
		size_t Write(void* data, size_t length);
		size_t Read(void* dest, size_t length);		
		size_t get_length() { return m_length; }

	};
}
#endif // !ORION_RESOURCE_H
