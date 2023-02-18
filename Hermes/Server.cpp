#include "Server.h"
#include <Psio/Log.h>

using std::list;

namespace Hermes
{
	int Server::Setup(std::string ip, uint16_t port, ConnectionHandler connection_handler)
	{
		int ret = 0;
		m_connection_handler = connection_handler;

		if (m_tcp)
		{
			if (!m_server)
				m_server = std::make_unique<Psio::TCPSocket>(m_net);
			ret = static_cast<Psio::TCPSocket*>(m_server.get())->Server(ip, port);
		}
		return ret;
	}

	void Server::Start()
	{
		int ret = m_server->Accept([&](Psio::StreamSocket* new_sock, int err) {
			if (err >= 0) {
				std::unique_ptr<Psio::StreamSocket> sock;
				sock.reset(new_sock);
				m_connection_handler(std::move(sock), err);
			}
			else {
				LOG(LOG_ERROR) << "Accept failed with error: " << err;
			}
			});
	}

	void Server::run()
	{
		m_net->RunLoop();
	}
}