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
#include <Messages/MessageConnect.h>

// test Hermes server and client stream

using ::testing::_;

using Psio::Network;

namespace Hermes
{
	// a dummy Stream Socket
	class MemorySocket : public Psio::StreamSocket
	{
	public:
		// write to socket
		virtual int Write(Psio::IOBuffer* buf, Socket::CompletionHandler handler) override
		{
			if (m_error < 1) {
				handler(m_error);
				return m_error;
			}
			int ret = (int)buf->Remaining();
			if (ret) {
				if (my_buffer->FreeBufSize() < ret)
					my_buffer->Grow(ret);
				memcpy(my_buffer->writer(), buf->reader(), ret);
				buf->Consume(ret);
				my_buffer->Produce(ret);
				handler(ret);
			}
			else
				ret = -1;
			
			return ret;
		};

		// read from socket
		virtual int Read(Psio::IOBuffer* buf, Socket::CompletionHandler handler) override
		{
			int ret = (int)my_buffer->Remaining();
			if (m_error < 1) {
				handler(m_error);
				return m_error;
			}
			if (ret) {
				if (buf->FreeBufSize() < ret)
					buf->Grow(ret);
				memcpy(buf->writer(), my_buffer->reader(), ret);
				my_buffer->Consume(ret);
				buf->Produce(ret);
				handler(ret);
			}
			else
				ret = -1;

			return ret;
		}

		virtual bool Close() override
		{
			return true;
		}

		virtual int Connect(Psio::SocketAddress& addr, bool blocking, Socket::CompletionHandler handler) override
		{
			handler(0);
			return 0;
		}

		virtual int Accept(ConnectHandler handler)
		{
			handler(this, 0);
			return 0;
		}

		virtual Psio::SocketAddress* getRemoteAddress()
		{
			return nullptr;
		}

		std::unique_ptr<Psio::IOBuffer> my_buffer = std::make_unique<Psio::IOBuffer>(64 * 1024);
		int m_error = 1;
	};

	// a fake Stream Service
	class StreamTest : public ::testing::Test {
	public:
		StreamTest() :
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
			m_factory->registerCoreMessages();
		}

		~StreamTest()
		{
		}

		int connections;
		bool do_quit;
		int connected;
		int messages;
		int messages_sent;
		bool login;

		std::unique_ptr<Stream> m_stream;

		//ConnectionHandler m_connection_handler;
		std::unique_ptr<Hermes::MessageFactory> m_factory;

		std::unique_ptr<Network> m_network;
		std::unique_ptr<Psio::StreamSocket> m_sock;
	};

	// tests

	TEST_F(StreamTest, SendSimpleMessage) {
		MemorySocket* sock = new MemorySocket();
		m_sock.reset(sock);

		m_stream = std::make_unique<Hermes::Stream>(std::move(m_sock), m_factory.get());

		testing::MockFunction<void(int err)> mock_handler;

		EXPECT_CALL(mock_handler, Call(_))
			.Times(1);

		std::unique_ptr<Hermes::Message> msg = std::make_unique<Hermes::Message>(1);
		int ret = m_stream->sendMessage(msg.get(), mock_handler.AsStdFunction());

		// Expect that 4 bytes are written
		EXPECT_EQ(ret, 4);
		Psio::IOBuffer* buf = sock->my_buffer.get();
		EXPECT_EQ(buf->Remaining(), 4);
	}

	TEST_F(StreamTest, ReadConnectMessage) {
		MemorySocket* sock = new MemorySocket();
		m_sock.reset(sock);

		m_stream = std::make_unique<Hermes::Stream>(std::move(m_sock), m_factory.get());

		std::unique_ptr<Hermes::MessageConnect> msg = std::make_unique<Hermes::MessageConnect>();
		
		testing::MockFunction<void(Hermes::MessageConnect* msg)> mock_connect_handler;

		EXPECT_CALL(mock_connect_handler, Call(_))
			.Times(1);

		int send_ret = m_stream->sendMessage(msg.get(), nullptr);

		m_stream->getMessageFactory()->registerMessageHandler<Hermes::MessageConnect>(HERMES_MSG_CONNECT, 
			nullptr,
			mock_connect_handler.AsStdFunction());

		int ret = m_stream->Start();
		EXPECT_EQ(ret, -1);
	}

	TEST_F(StreamTest, ReadMessageError) {
		MemorySocket* sock = new MemorySocket();
		m_sock.reset(sock);

		m_stream = std::make_unique<Hermes::Stream>(std::move(m_sock), m_factory.get());

		std::unique_ptr<Hermes::Message> msg = std::make_unique<Hermes::Message>(1);

		testing::MockFunction<void(int err)> mock_error_handler;

		EXPECT_CALL(mock_error_handler, Call(_))
			.Times(1);

		// force an error
		sock->m_error = -7;

		m_stream->onConnectionError(mock_error_handler.AsStdFunction());

		int ret = m_stream->Start();
		EXPECT_EQ(ret, -7);
	}

	TEST_F(StreamTest, ReadMessageClosed) {
		MemorySocket* sock = new MemorySocket();
		m_sock.reset(sock);

		m_stream = std::make_unique<Hermes::Stream>(std::move(m_sock), m_factory.get());

		std::unique_ptr<Hermes::Message> msg = std::make_unique<Hermes::Message>(1);

		testing::MockFunction<void(int err)> mock_error_handler;
		testing::MockFunction<void(int err)> mock_closed_handler;

		EXPECT_CALL(mock_error_handler, Call(_))
			.Times(1);
		EXPECT_CALL(mock_closed_handler, Call(_))
			.Times(1);

		// force an error
		sock->m_error = 0;

		m_stream->onConnectionError(mock_error_handler.AsStdFunction());
		m_stream->onClosed(mock_closed_handler.AsStdFunction());
		
		int ret = m_stream->Start();

		m_stream->Close();
		ret = m_stream->Restart();

		EXPECT_EQ(ret, -1);
	}

}