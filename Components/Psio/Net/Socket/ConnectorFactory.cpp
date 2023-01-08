#include "ConnectorFactory.h"
#include "TCPConnector.h"
#include "SSLConnector.h"
#include <Psio/Log.h>

namespace Psio
{
	ConnectorFactory& Psio::ConnectorFactory::factory()
	{
		static ConnectorFactory m_factory;
		return m_factory;
	}
	
	std::unique_ptr<Connector> ConnectorFactory::Create(Network* net, std::string scheme, std::string host, unsigned short port)
	{
		if (!scheme.compare("http")) {
			return std::make_unique<TCPConnector>(net, host, port);
		}
		else if (!scheme.compare("https")) {
			return std::make_unique<SSLConnector>(net, host, port);
		}

		LOG(LOG_ERROR) << "Unknown scheme";
		return nullptr;
	}
}