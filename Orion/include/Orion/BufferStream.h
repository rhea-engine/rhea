/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#ifndef ORION_BUFFERSTREAM_H
#define ORION_BUFFERSTREAM_H

#include <Orion/ResourceStream.h>
#include <Psio/Net/IOBuffer.h>

namespace orion
{
	class BufferResourceStream : public ResourceStream
	{
	public:
		BufferResourceStream(std::shared_ptr<psio::IOBuffer> buf) :
			m_buf(buf)
		{
			m_accessible = true;
			m_lastmod = 0;
		}
		~BufferResourceStream() {}
		size_t getLength() {
			return m_buf->GetSize();
		}
		void ReadAsync(psio::IOBuffer* buf) {
			buf->Grow(m_buf->GetSize());
			buf->Write(m_buf->reader(), m_buf->GetSize());
			if (m_data_handler)
				m_data_handler(buf->Remaining());
			if (m_end_handler)
				m_end_handler(buf->Remaining());
		}

		// resource stream no-ops

		// open for reading
		// used internally for async functions or must be called before using sync functions
		bool Open() { return true; };

		// write length bytes to resource
		// returns actual bytes written
		size_t Write(const void* data, size_t length) { return 0; }

		// read from resource up to length bytes

		// returns actual bytes read
		size_t Read(void* dest, size_t length) { return 0; }

		// get the next line as string
		std::string getLine() { return std::string(""); }

		// return eof condition
		bool eof() { return m_state == StreamState::STREAM_CLOSED; }

		// Close the stream
		void Close() { m_state = StreamState::STREAM_CLOSED; }

		// set internal mode
		void SetMode(int mode) { m_mode = mode; }

	protected:
		std::shared_ptr<psio::IOBuffer> m_buf;
		bool m_accessible = false;
		time_t m_lastmod = 0;
	};
}

#endif // !ORION_BUFFERSTREAM_H
