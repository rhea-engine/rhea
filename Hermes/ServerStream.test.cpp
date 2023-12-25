#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <string>
#include <Psio/Net/Network.h>
#include <Psio/Net/Socket/TCPSocket.h>

#include "ServerStream.h"
#include <Psio/Log.h>
#include <Opcode.h>
#include <MessageFactory.h>
#include <Messages/MessageConnect.h>

using ::testing::_;

using Psio::Network;

// a Mock Stream
class StreamMock : public Hermes::Stream
{
public:
	StreamMock(Hermes::MessageFactory* factory) :
		Stream(nullptr, factory)
	{}
	virtual int sendMessage(Hermes::Message* message, CompletionHandler handler)
	{
		messages.push_back(*message);
		handler(send_error);
		return -1;
	}
	virtual int Start() { return -1; }
	virtual void Close() { return; }

	int send_error = 0;
	std::vector<Hermes::Message> messages;
};

class ServerStreamTest : public ::testing::Test {
public:
	ServerStreamTest()
	{
		m_factory = std::make_unique<Hermes::MessageFactory>();
		m_factory->registerCoreMessages();

		m_stream = std::make_unique<StreamMock>(m_factory.get());
		m_hermes = std::make_unique<Hermes::ServerStream>(m_stream.get());
	}

	void SetUp() override
	{

	}

	~ServerStreamTest()
	{
	}

	std::unique_ptr<StreamMock> m_stream;
	std::unique_ptr<Hermes::MessageFactory> m_factory;
	std::unique_ptr<Hermes::ServerStream> m_hermes;
};

TEST_F(ServerStreamTest, Connect) {

	m_hermes->Initialize();
	m_hermes->Start();

	// client connects
	Hermes::MessageConnect connect;
	connect.world_name = "testworld";
	connect.protocol_version = HERMES_PROTOCOL_VERSION;

	m_hermes->getMessageFactory()->handle(&connect);

	// first message should be the HERMES_MSG_CONNECTED message
	EXPECT_EQ(m_stream->messages.size(), 1);
	EXPECT_EQ(m_stream->messages[0].base.opcode, HERMES_MSG_CONNECTED);
}

TEST_F(ServerStreamTest, ConnectAndLogin) {

	m_hermes->Initialize();
	m_hermes->Start();

	// client connects
	Hermes::MessageConnect connect;
	connect.world_name = "testworld";
	connect.protocol_version = HERMES_PROTOCOL_VERSION;
	m_hermes->getMessageFactory()->handle(&connect);

	Hermes::MessageLogin login;
	login.user_name = "anonymous";
	login.auth_method = "trust";
	m_hermes->getMessageFactory()->handle(&login);

	// send failure for anonymous login
	m_hermes->AuthFailure(nullptr, { "trust" });

	login.user_name = "testuser";
	m_hermes->getMessageFactory()->handle(&login);

	// send success for testuser login
	m_hermes->AuthSuccess(nullptr);

	// first message should be the HERMES_MSG_CONNECTED message
	EXPECT_EQ(m_stream->messages.size(), 3);
	EXPECT_EQ(m_stream->messages[0].base.opcode, HERMES_MSG_CONNECTED);
	EXPECT_EQ(m_stream->messages[1].base.opcode, HERMES_MSG_LOGIN_FAILED);
	EXPECT_EQ(m_stream->messages[2].base.opcode, HERMES_MSG_LOGIN_SUCCEEDED);
}

TEST_F(ServerStreamTest, ConnectAndCreateUser) {

	m_hermes->Initialize();
	m_hermes->Start();

	// client connects
	Hermes::MessageConnect connect;
	connect.world_name = "testworld";
	connect.protocol_version = HERMES_PROTOCOL_VERSION;
	m_hermes->getMessageFactory()->handle(&connect);

	Hermes::MessageLogin login;
	login.user_name = "anonymous";
	login.auth_method = "trust";
	m_hermes->getMessageFactory()->handle(&login);

	// send failure for anonymous login
	m_hermes->AuthFailure(nullptr, { "trust" });

	Hermes::MessageCreateUser create;
	create.user_name = "testuser";
	create.auth_method = "trust";
	m_hermes->getMessageFactory()->handle(&create);

	// send success for testuser login
	m_hermes->AuthSuccess(nullptr);

	// first message should be the HERMES_MSG_CONNECTED message
	EXPECT_EQ(m_stream->messages.size(), 3);
	EXPECT_EQ(m_stream->messages[0].base.opcode, HERMES_MSG_CONNECTED);
	EXPECT_EQ(m_stream->messages[1].base.opcode, HERMES_MSG_LOGIN_FAILED);
	EXPECT_EQ(m_stream->messages[2].base.opcode, HERMES_MSG_LOGIN_SUCCEEDED);
}

TEST_F(ServerStreamTest, FailLoginBeforeConnect) {

	m_hermes->Initialize();
	m_hermes->Start();

	// client connects
	Hermes::MessageConnect connect;
	connect.world_name = "testworld";
	connect.protocol_version = HERMES_PROTOCOL_VERSION;

	Hermes::MessageLogin login;
	login.user_name = "anonymous";
	login.auth_method = "trust";
	if (m_hermes->getMessageFactory()->filter(login.base.opcode)) {
		m_hermes->getMessageFactory()->handle(&login);

		// send failure for anonymous login
		m_hermes->AuthFailure(nullptr, { "trust" });
	}

	Hermes::MessageCreateUser create;
	create.user_name = "testuser";
	create.auth_method = "trust";
	if (m_hermes->getMessageFactory()->filter(create.base.opcode)) {
		m_hermes->getMessageFactory()->handle(&create);
		// send success for testuser login
		m_hermes->AuthSuccess(nullptr);
	}

	if (m_hermes->getMessageFactory()->filter(connect.base.opcode))
		m_hermes->getMessageFactory()->handle(&connect);

	// first message should be the HERMES_MSG_CONNECTED message - the others are ignored
	EXPECT_EQ(m_stream->messages.size(), 1);
	EXPECT_EQ(m_stream->messages[0].base.opcode, HERMES_MSG_CONNECTED);
}

