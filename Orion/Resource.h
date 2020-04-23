#ifndef ORION_RESOURCE_H
#define ORION_RESOURCE_H

#include <string>

using std::string;

namespace Orion
{
	class ResourceStream;

	class Resource
	{
	public:		
		virtual bool IsDir() { return false; }
		string get_mime() { return m_mime; }
		string get_name() { return m_name; }
		virtual size_t get_length() = 0;

		// open a stream to this resource		
		virtual ResourceStream* Open(int mode) = 0;

		// get as buffer

	protected:
		string m_mime;
		string m_name;
	};
}

#endif // !ORION_RESOURCE_H
