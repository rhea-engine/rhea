#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <string>
#include <Psio/Net/Network.h>
#include <Psio/Net/Socket/TCPSocket.h>

#include "ServerStream.h"
#include "ClientStream.h"
#include <Psio/Log.h>
#include <Opcode.h>
#include <MessageFactory.h>

// test Hermes server and client stream

using Psio::Network;

namespace Hermes
{
	// a fake Stream Service
	class StreamServiceTest : public ::testing::Test {
	public:
		StreamServiceTest() :
			connections(0),
			connected(0),
			messages(0),
			messages_sent(0),
			do_quit(false),
			login(true)
		{
			m_network = std::make_unique<Network>(nullptr);
			m_network->Init();
			m_factory = std::make_unique<Hermes::MessageFactory>();
		}
		~StreamServiceTest()
		{		
		}

		int StartServer(std::string ip, uint16_t port)
		{
			m_server_sock = std::make_unique<Psio::TCPSocket>(m_network.get());

			int err = m_server_sock->Server(ip, port);
			LOG(LOG_INFO) << "Started Server. Listening on " << ip << ":" << port;

			return m_server_sock->Accept([&](Psio::StreamSocket* new_sock, int err) {
				if (err < 0) {
					LOG(LOG_ERROR) << "OnConnection failed: " << err;
					return;
				}
				else {
					std::unique_ptr<Psio::StreamSocket> sock;
					sock.reset(new_sock);

					Hermes::ServerStream* hermes = new Hermes::ServerStream(std::move(sock), m_factory.get());
					int ret = hermes->onLogin([&,hermes](std::unique_ptr<Hermes::Message> msg, int err) {
						if (!err) {
							login = true;
							int ret = hermes->onMessage([&](std::unique_ptr<Hermes::Message> msg, int err) {
								if (!err) {
									messages++;
								}
								else {
									do_quit = true;
									delete hermes;
								}
								});
						}
						else {
							do_quit = true;
							delete hermes;
						}
						});
					if (ret < -1) {
						do_quit = true;
						delete hermes;
					}
				}
				});
		}

		typedef std::function<void(Hermes::ClientStream*)> ConnectionHandler;
		void connect(std::string host, uint16_t port, ConnectionHandler handler)
		{
			m_client_sock.reset(m_network->CreateTCPSocket());
			Psio::SocketAddress addr;
			addr.SetHost(host, port);
			m_connection_handler = handler;

			m_client_sock->Connect(addr, false, [&](int err) {
				connections++;
				if (!err) {
					Hermes::ClientStream* hermes = new Hermes::ClientStream(std::move(m_client_sock), m_factory.get());
					hermes->connect(nullptr);
					m_connection_handler(hermes);
				}
				else {
					LOG(LOG_ERROR) << "Connection failed: " << err;					
				}
				});
		}

		int connections;
		bool do_quit;
		int connected;
		int messages;
		int messages_sent;
		bool login;

		ConnectionHandler m_connection_handler;
		std::unique_ptr<Hermes::MessageFactory> m_factory;

		std::unique_ptr<Network> m_network;
		std::unique_ptr<Psio::TCPSocket> m_server_sock;
		std::unique_ptr<Psio::TCPSocket> m_client_sock;
	};

	// test the fake server
	TEST_F(StreamServiceTest, StartServer) {
		int ret = StartServer("127.0.0.1", 4242);
		EXPECT_EQ(ret, -1) << "Server was not started asynchronously";
	}

	// start server and connect
	TEST_F(StreamServiceTest, Connect) {
		int ret = StartServer("127.0.0.1", 4242);
		if (ret==-1) {
			connect("127.0.0.1", 4242, [&](Hermes::ClientStream* hermes) {
				{
					connected++;
					do_quit = true;
					delete hermes;
				}
				});
			do {
				// poll the accept loop
				m_network->RunLoop();
				
			} while (!do_quit);
		}
		EXPECT_EQ(ret, -1) << "Server was not started asynchronously";
		EXPECT_EQ(connections, 1) << "No connection received";
		EXPECT_EQ(connected, 1) << "Connection did not succeed";
	}

	// send login message
	TEST_F(StreamServiceTest, Login) {
		int ret = StartServer("127.0.0.1", 4242);
		if (ret == -1) {
			connect("127.0.0.1", 4242, [&](Hermes::ClientStream* hermes) {
				{
					connected++;					
					Message msg(HERMES_MSG_LOGIN);
					int ret = hermes->sendMessage(&msg, [&](int err) {
						messages_sent++;
						});
					if (ret > -1)
						messages_sent++;
				}
				});
			do {
				// poll the accept loop
				m_network->RunLoop();
				if (messages > 0)
					do_quit = true;

			} while (!do_quit && !login);
		}
		EXPECT_EQ(ret, -1) << "Server was not started asynchronously";
		EXPECT_EQ(connections, 1) << "No connection received";
		EXPECT_EQ(connected, 1) << "Connection did not succeed";
		EXPECT_EQ(messages_sent, 1) << "Did not send one message"; 
		//EXPECT_EQ(messages, 1) << "Did not receive one message";
		EXPECT_EQ(login, true) << "Did not login";
	}

	// send login message
	TEST_F(StreamServiceTest, OneMessage) {
		int ret = StartServer("127.0.0.1", 4242);
		if (ret == -1) {
			connect("127.0.0.1", 4242, [&](Hermes::ClientStream* hermes) {
				{
					connected++;
					Message msg(HERMES_MSG_LOGIN);
					int ret = hermes->sendMessage(&msg, [&](int err) {
						if (!err) {
							messages_sent++;
							hermes->sendMessage(&msg, [&](int err) {
								messages_sent++;
								});
						}
						});
					if (ret > -1) {
						messages_sent++;
						hermes->sendMessage(&msg, [&](int err) {
							messages_sent++;
							});
					}
				}
				});
			do {
				// poll the accept loop
				m_network->RunLoop();
				if (messages > 0)
					do_quit = true;

			} while (!do_quit && messages<1);
		}
		EXPECT_EQ(ret, -1) << "Server was not started asynchronously";
		EXPECT_EQ(connections, 1) << "No connection received";
		EXPECT_EQ(connected, 1) << "Connection did not succeed";
		EXPECT_EQ(messages_sent, 1) << "Did not send one message";
		EXPECT_EQ(messages, 1) << "Did not receive one message";
		EXPECT_EQ(login, true) << "Did not login";
	}
}