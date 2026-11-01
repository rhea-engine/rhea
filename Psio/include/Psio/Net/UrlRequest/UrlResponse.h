/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#ifndef PSIO_NET_URLRESPONE_H
#define PSIO_NET_URLRESPONE_H

#include <Psio/IO/Stream.h>
#include <Psio/Net/IOBuffer.h>
#include <Psio/Net/UrlRequest/Url.h>

namespace psio
{
	class HttpTransaction;

	class UrlResponse : public Stream
	{
	public:
		UrlResponse(HttpTransaction* transaction, Url url, int response_code) :
			m_transaction(transaction),
			m_buffer(nullptr),
			m_response_code(response_code),
			m_size(0),
			m_effective_url(url)
		{
			m_state = StreamState::STREAM_OPEN;
		}
		~UrlResponse();

		// send chunk to listeners
		size_t Push(char* chunk, size_t size);

		// called when data is read - a chunk of the data is passed to handler until all data is sent
		void ReadAsync(psio::IOBuffer* buf);

		// no modes for url stream
		void SetMode(int mode) {}

		int GetResponseCode() {	return m_response_code;	}

		size_t getLength() { return m_size; }

		Url getEffectiveUrl() { return m_effective_url; }
		void setEffectiveUrl(Url url) { m_effective_url = url; }

	protected:
		HttpTransaction* m_transaction;
		IOBuffer* m_buffer;
		int m_response_code;
		size_t m_size;
		Url m_effective_url;

		// read one more chunk
		void doReadDataLoop();
		void doReadData(int result);
	};
}

#endif // !PSIO_NET_URLRESPONE_H