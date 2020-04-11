#ifndef ORION_RESOURCESTREAM_H
#define ORION_RESOURCESTREAM_H

#include "Resource.h"

namespace Orion
{
	class ResourceStream
	{
	protected:
		Resource* m_resource;
		bool m_isOpen;		
	public:		
		// write length bytes to resource
		// returns actual bytes written
		virtual size_t Write(void* data, size_t length) = 0;

		// read from resource up to length bytes
		// returns actual bytes read
		virtual size_t Read(void* dest, size_t length) = 0;

		// get the next line as string
		virtual string getLine() = 0;

		// return eof condition
		virtual bool eof() = 0;

		// Close the stream
		virtual void Close() = 0;
	};
}

#endif // !ORION_RESOURCESTREAM_H
