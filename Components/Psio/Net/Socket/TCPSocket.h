#ifndef PSIO_NET_TCPSOCKET_H
#define PSIO_NET_TCPSOCKET_H

#include "../Network.h"
#include "StreamSocket.h"
#include "SocketAddress.h"
#include "../IOBuffer.h"

namespace Psio
{
	class TCPSocket : public StreamSocket
	{
	public:
		TCPSocket(Network* net, SOCKET sock) :
			m_socket(sock),
			m_net(net),
			m_wait_connect(true),
			m_connect_state(ConnectState::CONNECT_STATE_UNSET),
			m_next_connect_state(ConnectState::CONNECT_STATE_UNSET),
			m_isConnected(false)
		{}
		TCPSocket(Network* net) :
			TCPSocket(net, BAD_SOCKET)
		{}
		~TCPSocket();

		// Create a socket with the given family
		void SetAddress(SocketAddress& addr);
		SocketAddress GetAddress();

		int Open();
		bool Close();

		// set the socket to non blocking
		int SetNonBlocking();

		// set the socket to blocking
		int SetBlocking();

		// StreamSocket implementations

		int Connect(SocketAddress &addr, bool blocking, Socket::CompletionHandler handler);

		// the connection loop
		int DoConnectLoop(int result);

		// server functions

		// setup a server on ip and port
		int Server(std::string ip, int port, bool blocking = false);

		// bind socket
		int Bind();
		// listen on address
		int Listen(int backlog);

		// accept a socket connection
		int Accept(ConnectHandler handler);

		// socket implementation
		int Write(IOBuffer* buf, Socket::CompletionHandler handler);
		int Read(IOBuffer* buf, Socket::CompletionHandler handler);
		SocketAddress* getRemoteAddress() { return &m_addr; }

		// get raw socket
		SOCKET GetRawSocket() { return m_socket; }

	protected:
		bool m_wait_connect;
		Socket::CompletionHandler m_connect_complete_handler;

		SocketAddress m_addr;

		// states fro connection
		enum class ConnectState {
			CONNECT_STATE_UNSET,
			CONNECT_STATE_CONNECT,
			CONNECT_STATE_COMPLETED,
			CONNECT_STATE_DONE
		};
		ConnectState m_connect_state;
		ConnectState m_next_connect_state;

		// handle connect start state
		int DoConnect();
		// handle connect completed state
		int DoConnectCompleted(int result);

		Network* m_net;
		SOCKET m_socket;

		bool m_isConnected;
	};
}

#endif // !PSIO_NET_TCPSOCKET_H
