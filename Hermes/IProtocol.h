#ifndef Hermes_IPROTOCOL_H
#define Hermes_IPROTOCOL_H

namespace Hermes
{
	class Connection;

	class IProtocol
	{
	public:
		virtual ~IProtocol() {}
		// call Protocol on connection with the connection object
		virtual void onConnection(Connection* conn) = 0;
		// data is available
		virtual void onData(unsigned char* buf, size_t buflen) = 0;
	};
}

#endif // !Hermes_IPROTOCOL_H
