#include "ConnectorFactory.h"
#include "TCPConnector.h"
#ifdef HAS_OPENSSL
#include "SSLConnector.h"
#endif
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
#ifdef HAS_OPENSSL
		else if (!scheme.compare("https")) {
			return std::make_unique<SSLConnector>(net, host, port);
		}
#endif
		LOG(LOG_ERROR) << "Unknown scheme";
		return nullptr;
	}
}