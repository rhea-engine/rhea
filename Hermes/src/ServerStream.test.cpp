#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <string>
#include <Psio/Net/Network.h>
#include <Psio/Net/Socket/TCPSocket.h>

#include "Hermes/ServerStream.h"
#include <Psio/Log.h>
#include <Hermes/Opcode.h>
#include <Hermes/MessageFactory.h>
#include <Hermes/Messages/MessageConnect.h>

using ::testing::_;

using psio::Network;

// a Mock Stream
class StreamMock : public hermes::Stream
{
public:
	StreamMock(hermes::MessageFactory* factory) :
		Stream(nullptr, factory)
	{}
	virtual int sendMessage(hermes::Message* message, CompletionHandler handler)
	{
		messages.push_back(*message);
		handler(send_error);
		return -1;
	}
	virtual int Start() { return -1; }
	virtual void Close() { return; }

	int send_error = 0;
	std::vector<hermes::Message> messages;
};

class ServerStreamTest : public ::testing::Test {
public:
	ServerStreamTest()
	{
		m_factory = std::make_unique<hermes::MessageFactory>();
		m_factory->registerCoreMessages();

		m_stream = std::make_unique<StreamMock>(m_factory.get());
		m_hermes = std::make_unique<hermes::ServerStream>(m_stream.get());
	}

	void SetUp() override
	{

	}

	~ServerStreamTest()
	{
	}

	std::unique_ptr<StreamMock> m_stream;
	std::unique_ptr<hermes::MessageFactory> m_factory;
	std::unique_ptr<hermes::ServerStream> m_hermes;
};

TEST_F(ServerStreamTest, ConnectAndLogin) {

	m_hermes->Initialize();
	m_hermes->Start();

	hermes::MessageLogin login;
	login.user_name = "anonymous";
	login.auth_method = "trust";
	m_stream->getMessageFactory()->handle(&login);

	// send failure for anonymous login
	m_hermes->AuthFailure(nullptr, { "trust" });

	login.user_name = "testuser";
	m_stream->getMessageFactory()->handle(&login);

	// send success for testuser login
	m_hermes->AuthSuccess(nullptr);

	// first message should be the HERMES_MSG_CONNECTED message
	ASSERT_EQ(m_stream->messages.size(), 2);
	EXPECT_EQ(m_stream->messages[0].base.opcode, HERMES_MSG_LOGIN_FAILED);
	EXPECT_EQ(m_stream->messages[1].base.opcode, HERMES_MSG_LOGIN_SUCCEEDED);
}

TEST_F(ServerStreamTest, ConnectAndCreateUser) {

	m_hermes->Initialize();
	m_hermes->Start();

	hermes::MessageLogin login;
	login.user_name = "anonymous";
	login.auth_method = "trust";
	m_stream->getMessageFactory()->handle(&login);

	// send failure for anonymous login
	m_hermes->AuthFailure(nullptr, { "trust" });

	hermes::MessageCreateUser create;
	create.user_name = "testuser";
	create.auth_method = "trust";
	m_stream->getMessageFactory()->handle(&create);

	// send success for testuser login
	m_hermes->AuthSuccess(nullptr);

	// first message should be the HERMES_MSG_CONNECTED message
	ASSERT_EQ(m_stream->messages.size(), 2);
	EXPECT_EQ(m_stream->messages[0].base.opcode, HERMES_MSG_LOGIN_FAILED);
	EXPECT_EQ(m_stream->messages[1].base.opcode, HERMES_MSG_LOGIN_SUCCEEDED);
}