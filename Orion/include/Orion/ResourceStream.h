/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#ifndef ORION_RESOURCESTREAM_H
#define ORION_RESOURCESTREAM_H

#include <functional>
#include <string>
#include <Psio/IO/Stream.h>

namespace orion
{
	class ResourceStream : public psio::Stream
	{
	public:		
		ResourceStream() :
			m_mode(1),
			psio::Stream()
		{}
		virtual ~ResourceStream() {}

		virtual bool IsDir() { return false; }
		virtual std::string get_mime() { return m_mime; }
		virtual std::string get_name() { return m_name; }
		virtual size_t getLength() = 0;

		// open for reading
		// used internally for async functions or must be called before using sync functions
		virtual bool Open() = 0;

		// write length bytes to resource
		// returns actual bytes written
		virtual size_t Write(const void* data, size_t length) = 0;

		// read from resource up to length bytes

		// returns actual bytes read
		virtual size_t Read(void* dest, size_t length) = 0;

		// get the next line as string
		virtual std::string getLine() = 0;

		// return eof condition
		virtual bool eof() = 0;

		// Close the stream
		virtual void Close() = 0;

	protected:		
		int m_mode;
		std::string m_mime;
		std::string m_name;
	};
}

#endif // !ORION_RESOURCESTREAM_H
