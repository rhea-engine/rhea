#ifndef PSIO_NET_IOBUFFER_H
#define PSIO_NET_IOBUFFER_H

#include <cstddef>

namespace Psio
{
	class IOBuffer
	{
	public:
		IOBuffer() :
			m_buffer(nullptr),
			m_size(0),
			m_offset(0),
			m_write_offset(0)
		{}
		IOBuffer(size_t size);
		~IOBuffer();

		// get data
		char* data() const { return m_buffer; }

		// detach buffer (used for copying the buffer)
		void DetachBuffer() { m_buffer = nullptr; }

		// get data with reader pointer
		char* reader() const { return m_buffer + m_offset; }

		// get data with writer pointer
		char* writer() const { return m_buffer + m_write_offset; }

		// consume (read) bytes
		void Consume(size_t bytes);

		// write bytes
		void Produce(size_t bytes) { m_write_offset += bytes; }

		// write chunk
		void Write(const char* chunk, size_t size);

		// set offset to read from
		void SetOffset(int offset) { m_offset = offset; }

		// remaining data to read
		size_t Remaining() { return m_write_offset - m_offset; }

		// Remaining Buffer for writing
		size_t FreeBufSize() { return m_size - m_write_offset; }
		
		// grow buffer
		void Grow(size_t size);

		size_t GetSize() { return m_size; }

		char& operator[](int) { return m_buffer[m_write_offset]; }
		char operator[](int) const { return m_buffer[m_offset]; }

	protected:
		char* m_buffer;
		size_t m_size;
		size_t m_offset;
		size_t m_write_offset;
	};
}
#endif // !PSIO_NET_IOBUFFER_H
