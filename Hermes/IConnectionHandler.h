#ifndef HERMES_ICONNECTRION_HANDLER_H
#define HERMES_ICONNECTRION_HANDLER_H

namespace Hermes
{
	class Connection;

	class IConnectionHandler
	{
	public:
		virtual void onConnection(Connection *conn) = 0;
		virtual void onClose(Connection* conn) = 0;
	};
}

#endif // !HERMES_ICONNECTRION_HANDLER_H