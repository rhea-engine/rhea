/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#ifndef PSIO_CONNECTOR_H
#define PSIO_CONNECTOR_H

#include <string>
#include "../Network.h"
#include "Socket.h"

namespace psio
{
	/*
	* Connector is an abstract interface to connectors to sockets
	* The connection should establish all necessary negotations to allow 
	* a stream to processed over it. The connector may return a specialized
	* socket (like SSLSocket)
	*/
	class Connector
	{
	public:		
		virtual ~Connector() {};

		virtual int Start(Network::CompletionHandler handler) = 0;

		// return the socket and transfer ownership
		virtual std::unique_ptr<Socket> GetSocket() = 0;
	};
}

#endif // !PSIO_CONNECTOR_H
