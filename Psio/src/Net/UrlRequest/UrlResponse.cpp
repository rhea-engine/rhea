/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#include <Psio/Net/UrlRequest/UrlResponse.h>
#include <cassert>
#include <Psio/Net/Http/HttpTransaction.h>

// Chunk size is 64 kB
#define CHUNK_SIZE 64 * 1024 

namespace psio
{
	UrlResponse::~UrlResponse()
	{
	}

	size_t UrlResponse::Push(char* chunk, size_t size)
	{
		if (m_buffer) {
			// store to the receives buffer
			m_buffer->Write(chunk, size);
			// call data handler if defined
			if (!m_data_handler) {
				m_data_handler(size);
			}
			return size;
		}
		else
			// if we have no buffer - pause the stream
			return 0;
	}

	void UrlResponse::ReadAsync(psio::IOBuffer* buf)
	{
		if (!m_buffer)
			m_buffer = buf;
		doReadDataLoop();
	}

	void UrlResponse::doReadDataLoop()
	{
		// get data from transaction
		int result = m_transaction->ReadBody(m_buffer, CHUNK_SIZE, [this](int result) {
			doReadData(result);
			});
		if (result != Network::Err::IO_PENDING)
			doReadData(result);
	}

	void UrlResponse::doReadData(int result) {
		// data available and stremaing requested?
		if (m_data_handler) {
			m_data_handler(result);
		}

		if (result > 0) {
			m_size += result;
			// check for more data
			doReadDataLoop();
		}
		else {			
			// network error - call error handler
			if (result < 0) {
				m_state = StreamState::STREAM_ERROR;
				if (m_error_handler)
					m_error_handler(result);
			}
			else {
				// EOF reach call end handler
				m_state = StreamState::STREAM_CLOSED;
				if (m_end_handler) {
					m_end_handler(m_buffer->Remaining());
				}
			}
		}
	}
}