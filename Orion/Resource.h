#ifndef ORION_RESOURCE_H
#define ORION_RESOURCE_H

#include <string>

using std::string;

namespace Orion
{
	class Resource
	{
	protected:
		string m_mime;
		string m_name;
	public:		
		virtual size_t Write(void* data, size_t length) = 0;
		virtual size_t Read(void* dest, size_t* length = 0) = 0;
		virtual bool IsDir() { return false; }
		string get_mime() { return m_mime; }
		string get_name() { return m_name; }
		virtual size_t get_length() = 0;
	};
}

#endif // !ORION_RESOURCE_H
