#ifndef ORION_FILERESOURCE_H
#define ORION_FILERESOURCE_H

#include <string>

#include "Resource.h"

using std::string;

namespace Orion
{
	class FileResource : public Resource
	{
	protected:
		void* m_data;
		size_t m_length;
		size_t m_buf_length;
	public:
		FileResource();
		~FileResource();
		size_t Write(void* data, size_t length);
		size_t Read(void* dest, size_t* length = 0);
		size_t get_length() { return m_length; }

	};
}
#endif // !ORION_FILERESOURCE_H
