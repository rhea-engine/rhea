#include <openssl/crypto.h> 
#include <openssl/x509.h> 
#include <openssl/pem.h> 
#include <openssl/ssl.h> 
#include <openssl/err.h> 

#include "SSLConnector.h"

namespace Psio
{
	SSLConnector::SSLConnector(Network* net, std::string& host, unsigned short port) :
		m_net(net),
		m_host(host),
		m_port(port),
		m_socket(nullptr),
		m_tp_socket(nullptr)
	{
	}

	SSLConnector::~SSLConnector()
	{
	}

	int SSLConnector::Start(Network::CompletionHandler handler)
	{
		// first create a tcp endpoint and connect it
		m_tp_socket = std::make_unique<TCPSocket>(m_net);
		SocketAddress m_s_addr;

		if (m_host == "")
			m_s_addr.SetHost("localhost", m_port);
		else
			m_s_addr.SetHost(m_host, m_port);

		m_socket = std::make_unique<SSLSocket>(m_net, std::move(m_tp_socket), nullptr);

		int ret = m_socket->Connect(m_s_addr, false, [handler](int result) -> void {			
			handler(result);
			});
		return ret;
	}
}