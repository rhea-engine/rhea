#ifndef PSIO_NET_URLRESPONE_H
#define PSIO_NET_URLRESPONE_H

#include "../../Stream.h"
#include "../IOBuffer.h"

namespace Psio
{
	class HttpTransaction;

	class UrlResponse : public Stream
	{
	public:
		UrlResponse(HttpTransaction* transaction, int response_code) :
			m_transaction(transaction),
			m_buffer(nullptr),
			m_response_code(response_code),
			m_size(0)
		{}
		~UrlResponse();

		// send chunk to listeners
		size_t Push(char* chunk, size_t size);

		// called when data is read - a chunk of the data is passed to handler until all data is sent
		void ReadAsync(Psio::IOBuffer* buf);

		// no modes for url stream
		void SetMode(int mode) {}

		int GetResponseCode() {	return m_response_code;	}

	protected:
		HttpTransaction* m_transaction;
		IOBuffer* m_buffer;
		int m_response_code;
		size_t m_size;

		// read one more chunk
		void doReadDataLoop();
		void doReadData(int result);
	};
}

#endif // !PSIO_NET_URLRESPONE_H