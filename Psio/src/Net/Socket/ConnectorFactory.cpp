/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#include <Psio/Net/Socket/ConnectorFactory.h>
#include <Psio/Net/Socket/TCPConnector.h>
#ifdef HAS_OPENSSL
#include <Psio/Net/Socket/SSLConnector.h>
#endif
#include <Psio/Log.h>

namespace psio
{
	ConnectorFactory& psio::ConnectorFactory::factory()
	{
		static ConnectorFactory m_factory;
		return m_factory;
	}
	
	std::unique_ptr<Connector> ConnectorFactory::Create(Network* net, std::string scheme, std::string host, unsigned short port)
	{
		if (!scheme.compare("http")) {
			return std::make_unique<TCPConnector>(net, host, port);
		}
#ifdef HAS_OPENSSL
		else if (!scheme.compare("https")) {
			return std::make_unique<SSLConnector>(net, host, port);
		}
#endif
		LOG(LOG_ERROR) << "Unknown scheme";
		return nullptr;
	}
}