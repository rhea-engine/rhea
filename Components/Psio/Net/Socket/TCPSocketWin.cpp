#include "TCPSocket.h"
#include "SocketAddress.h"
#include "../Network.h"
#include <Psio/Log.h>

namespace Psio
{
	TCPSocket::~TCPSocket()
	{
		if (m_isConnected)
			Close();
	}

	void TCPSocket::SetAddress(SocketAddress& addr)
	{
		m_addr = addr;
	}

	int TCPSocket::Open()
	{
		m_socket = socket(m_addr.getFamily(), SOCK_STREAM, 0);
		if (m_socket == BAD_SOCKET) {
			int rv = WSAGetLastError();
			return m_net->SysErr(rv);
		}
		else
			m_isConnected = true;
		return 0;
	}

	bool TCPSocket::Close()
	{
		m_net->UnWatch(m_socket);
		closesocket(m_socket);
		m_socket = BAD_SOCKET;
		m_isConnected = false;
		return false;
	}

	int TCPSocket::SetNonBlocking()
	{
		unsigned long val = LOCAL_NONBLOCK;
		unsigned lanswer = 0;
		lanswer = ioctlsocket(m_socket, LOCAL_FL_SET, &val);
		if (lanswer != 0) {
			return -1;
		}
		return 0;
	}

	int TCPSocket::SetBlocking()
	{
		unsigned long val = 0;
		unsigned lanswer = 0;
		lanswer = ioctlsocket(m_socket, LOCAL_FL_SET, &val);
		if (lanswer != 0) {
			return -1;
		}
		return 0;
	}

	int TCPSocket::Connect(SocketAddress& addr, bool blocking, Socket::CompletionHandler handler)
	{
		m_socket = socket(addr.getFamily(), SOCK_STREAM, 0);
		if (m_socket == BAD_SOCKET) {
			return Network::Err::FAILED;
		}
		
		if (!blocking)
			SetNonBlocking();

		m_connect_complete_handler = handler;
		m_addr = addr;
		m_isConnected = true;

		m_next_connect_state = ConnectState::CONNECT_STATE_CONNECT;
		return DoConnectLoop(0);
	}

	int TCPSocket::DoConnectLoop(int result)
	{
		int err = result;
		do
		{
			m_connect_state = m_next_connect_state;

			switch (m_connect_state)
			{
			case ConnectState::CONNECT_STATE_CONNECT:
				err = DoConnect();
				break;

			case ConnectState::CONNECT_STATE_COMPLETED:
				err = DoConnectCompleted(err);
				break;
			}

		} while ((err != Network::Err::IO_PENDING) && (m_connect_state != ConnectState::CONNECT_STATE_UNSET && m_connect_state != ConnectState::CONNECT_STATE_DONE));

		return err;
	}

	int TCPSocket::Server(std::string ip, int port, bool blocking)
	{
		m_addr.SetHost(ip, port);
		int ret = Open();
		if (ret)
			return ret;
		if (!blocking)
			SetNonBlocking();
		ret = Bind();
		if (ret)
			return ret;
		ret = Listen(5);
		if (ret)
			return ret;

		return 0;
	}
	
	int TCPSocket::Bind()
	{
		int ret = bind(m_socket, (SOCKADDR*)&m_addr.GetAddressInfo(), (m_addr.getFamily()==AF_INET) ? sizeof(SOCKADDR_IN) : sizeof(SOCKADDR_IN6));
		if (ret == SOCKET_ERROR) {
			int rv = WSAGetLastError();
			return m_net->SysErr(rv);
		}
		return 0;
	}

	int TCPSocket::DoConnect()
	{
		if (connect(m_socket, &m_addr.GetAddressInfo(), SA_SIZEOF(&m_addr.GetAddressInfo())) == SOCKET_ERROR) {
			if (GetLastError() != LOCAL_CONNECT_BLOCKING) {
				printf("Non Blocking Connect Error %d", GetLastError());
				Close();
				return Network::Err::FAILED;
			}
			// register listener
			m_net->Watch(m_socket, Network::IOFLAG::WRITE | Network::IOFLAG::EXCEPT, true, [&](int result) {
				if (result & Network::IOFLAG::EXCEPT)
					result = Network::Err::FAILED;
				else
					result = Network::Err::OK;
				result = DoConnectLoop(result);

				// call callback - this may destroy this class, therefore it must be called last
				if (result != Network::Err::IO_PENDING)
					m_connect_complete_handler(result);
			});
			m_next_connect_state = ConnectState::CONNECT_STATE_COMPLETED;
			return Network::Err::IO_PENDING;
		}
		// synchonous result - should not happen, but would be ok
		m_next_connect_state = ConnectState::CONNECT_STATE_COMPLETED;
		return Network::Err::OK;
	}

	int TCPSocket::DoConnectCompleted(int result)
	{
		m_next_connect_state = ConnectState::CONNECT_STATE_DONE;
		return result;
	}

	int TCPSocket::Listen(int backlog)
	{
		int ret = listen(m_socket, backlog);
		if (ret) {
			ret = WSAGetLastError();
			return m_net->SysErr(ret);
		}
		return 0;
	}

	int TCPSocket::Accept(ConnectHandler handler)
	{
		SOCKET connected_socket = accept(m_socket, NULL, NULL);
		if (connected_socket == INVALID_SOCKET) {
			int ret = WSAGetLastError();
			if (ret == WSA_IO_PENDING || ret == WSAEWOULDBLOCK) {
				// non blocking
				DLOG(LOG_DEBUG) << "watch for incomming connection";
				m_net->Watch(m_socket, Network::IOFLAG::READ, false, [&, handler](int result) {
					if (result >= 0)
						result = Network::Err::OK;
					SOCKET connected_socket = accept(m_socket, NULL, NULL);
					DLOG(LOG_DEBUG) << "New socket: " << connected_socket;
					if (connected_socket == INVALID_SOCKET) {
						result = WSAGetLastError();
						handler(nullptr, result);
					}
					else {
						TCPSocket* new_sock = new TCPSocket(m_net, connected_socket);
						handler(new_sock, result);
					}
					});
				return Network::Err::IO_PENDING;
			}
			return m_net->SysErr(ret);
		}
		return 0;
	}

	int TCPSocket::Write(IOBuffer* buf, Socket::CompletionHandler handler)
	{
		int ret = send(m_socket, buf->reader(), (int)buf->Remaining(), 0);
		if (ret == INVALID_SOCKET) {
			int err = WSAGetLastError();
			if (err == WSAEWOULDBLOCK) {
				// non-blocking write
				m_net->Watch(m_socket, Network::IOFLAG::WRITE, true, [&, handler, buf](int result)->void {
					int ret = send(m_socket, buf->reader(), (int)buf->Remaining(), 0);
					if (ret == INVALID_SOCKET) {
						int err = WSAGetLastError();
						ret = m_net->SysErr(err);
					}
					else if (ret > 0)
						buf->Consume(ret);
					 handler(ret);
					});
				return Network::Err::IO_PENDING;
			}
			else {
				return m_net->SysErr(err);
			}
		}
		else if (ret > 0)
			buf->Consume(ret);
		return ret;
	}

	int TCPSocket::Read(IOBuffer* buf, Socket::CompletionHandler handler)
	{
		if (buf->FreeBufSize() < 8 * 1024)
			buf->Grow(64 * 1024);
		int ret = recv(m_socket, buf->writer(), (int)buf->FreeBufSize(), 0);
		if (ret == INVALID_SOCKET) {
			int err = WSAGetLastError();
			if (err == WSAEWOULDBLOCK) {
				// non-blocking read
				m_net->Watch(m_socket, Network::IOFLAG::READ, true, [&, handler, buf](int result)->void {
					int ret = recv(m_socket, buf->writer(), (int)buf->FreeBufSize(), 0);
					if (ret == INVALID_SOCKET) {
						int err = WSAGetLastError();
						ret = m_net->SysErr(err);
					}
					if (ret>0)
						buf->Produce(ret);
					handler(ret);
					});
				return Network::Err::IO_PENDING;
			}
			else {
				return m_net->SysErr(err);
			}
		}
		else if (ret > 0)
			buf->Produce(ret);
		return ret;
	}
}