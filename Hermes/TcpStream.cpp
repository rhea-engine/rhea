#include "TcpStream.h"

namespace Hermes
{
	TcpStream::~TcpStream()
	{
		if (m_socket)
			delete m_socket;
	}

	int TcpStream::accept(Socket* socket, IPAddress remote_ip, uint16_t rport)
	{
		m_socket = socket;
		m_remote_addr.ip = remote_ip;
		m_remote_addr.port = rport;
		return 0;
	}

	int TcpStream::read(uint8_t* buffer, int bytes, int& received)
	{
		if (!m_socket->is_open())
			return -1;

		int buflen = bytes;
		received = 0;

		int read = 0;
		int ret = m_socket->receive(buffer, buflen, read);
		if (ret > 0)
			received = read;
		else
			received = 0;
		return ret;
	}

	int TcpStream::write(uint8_t* buffer, int bytes, int& sent)
	{
		if (!m_socket->is_open())
			return -1;

		int buflen = bytes;
		sent = 0;

		int written = 0;
		int ret = m_socket->send(buffer, buflen, written);
		if (ret > 0)
			sent = written;
		else
			sent = 0;
		return ret;
	}

	bool TcpStream::inputPending()
	{
		if (!m_socket->is_open())
			return false;

		int ret = m_socket->poll(true, false, 0);
		if (ret < 0)
			m_socket->close();
		if (ret == 1)
			return true;
		return false;
	}
	
	void TcpStream::close()
	{
		m_socket->close();
	}
}