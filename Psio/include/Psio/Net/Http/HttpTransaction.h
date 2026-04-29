/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#ifndef PSIO_HTTP_TRANSACTION_H
#define PSIO_HTTP_TRANSACTION_H

#include "../Network.h"
#include "HttpRequest.h"

#include <string>

namespace psio
{
	class IOBuffer;

	class HttpTransaction
	{
	public:
		virtual ~HttpTransaction() {}

		/* Start a Http transaction.
		*
		* This sets up a http stream and sends the request and reads the headers
		*
		* If IO_PENDING is returned handler is called as soon as the headers are read
		* Any other negative return value indicates an error
		*/
		virtual int Start(HttpRequest& req, Network::CompletionHandler handler) = 0;

		/* Read Body chunk
		*
		*  Read up to chunk_len bytes into buffer buf.
		*
		* If IO_PENDING is returned handler is called on completion of read
		* Any other negative return value indicates an error
		*/
		virtual int ReadBody(IOBuffer* buf, size_t chunk_len, Network::CompletionHandler handler) = 0;

		/* Return the Http headers 
		*/
		virtual HttpHeaders* GetHeaders() = 0;
	};
}

#endif // !PSIO_HTTP_TRANSACTION_H
