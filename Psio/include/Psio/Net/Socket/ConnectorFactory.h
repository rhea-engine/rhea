#ifndef PSIO_NET_CONNECTORFACTORY_H
#define PSIO_NET_CONNECTORFACTORY_H

#include "Connector.h"
#include "../Network.h"

namespace psio
{
	class ConnectorFactory
	{
	public:

		// Singleton
		static ConnectorFactory& factory();

		std::unique_ptr<Connector> Create(Network* net, std::string scheme, std::string host, unsigned short port);

	protected:
		
		ConnectorFactory()
		{
		}		
	};
}

#endif // !PSIO_NET_CONNECTORFACTORY_H
