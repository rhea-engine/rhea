#include "gtest/gtest.h"
#include <gmock/gmock.h>
#include "ClientConnection.h"
#include <Rhea/Realm.h>
#include <Rhea/UnitTree.h>
#include <Rhea/Fabrics/Core/Scene.h>
#include <Hermes/ServerStream.h>
#include <Psio/Net/Socket/TCPSocket.h>
#include <Hermes/Opcode.h>
#include <Hermes/Messages/MessageEnterWorld.h>
#include <Rhea/Fabrics/3D/Item.h>
#include <Hermes/Messages/MessageMove.h>
#include <Hermes/Messages/MessageTurn.h>
#include <Rhea/Fabrics/Core/Avatar.h>
#include "ServerService.h"
#include "Accounts/Provider.h"
#include "Accounts/AccountService.h"
#include "Accounts/ProviderMemory.h"

using ::testing::_;

class TestApp : public Rhea::Application
{
public:
    void onScene(Rhea::Scene* scene)
    {}
	~TestApp() {}
};

class WorldMock : public World
{
	MECLASS(WorldMock, Rhea::Unit)
public:
	WorldMock(std::string name, std::string type) :
		World(name, type)
	{}
	~WorldMock() { }
		
	MOCK_METHOD(void, setScene, (std::string));
	MOCK_METHOD(std::string, getScene, ());	
};

class HermesServerMock : public Hermes::ServerStream
{
public:
	HermesServerMock(std::unique_ptr<Psio::StreamSocket> sock, Hermes::MessageFactory* msg_factory) :
		Hermes::ServerStream(std::move(sock), nullptr)
	{}
	~HermesServerMock() { }

	MOCK_METHOD(int, onLogin, (ServerStream::LoginHandler handler));
	MOCK_METHOD(int, onMessage, (ServerStream::MessageHandler handler));
	MOCK_METHOD(int, sendMessage, (Hermes::Message*, Hermes::Stream::CompletionHandler));
};

class PlayerMock : public Rhea::Item
{
	MECLASS(PlayerMock, Rhea::Item)
public:
	PlayerMock(std::string name, std::string type) :
		Rhea::Item(name, type)
	{}
	~PlayerMock() { }

	MOCK_METHOD(void, setMovement, (Math::Vector3));
	MOCK_METHOD(void, setTurn, (float));
};

class ServerServiceMock : public ServerService
{
	MECLASS(ServerServiceMock, ServerService)
public:
	ServerServiceMock(std::string name, std::string type) :
		ServerService(name, type)
	{}
	~ServerServiceMock() { }

	MOCK_METHOD(void, broadcast, (Hermes::Message*, ClientConnection*));
};

class ClientConnectionTest : public ::testing::Test {
protected:
	TestApp* app = nullptr;
	Rhea::Realm* realm = nullptr;
	Rhea::Scene* scene = nullptr;	
	std::unique_ptr<HermesServerMock> hermes;
	Hermes::ServerStream::MessageHandler handler;
	std::unique_ptr<PlayerMock> player;
	std::unique_ptr<ServerServiceMock> server;
	std::unique_ptr<AccountService> acc_srv;

	void SetUp() override {
		// add an app
		app = new TestApp();
		app->init();
		scene = new Rhea::Scene("testscene", "//meworla/scene");

		SimpleParser::Type::registerType<ServerServiceMock>("//meworla/serverservice");
		SimpleParser::Type::registerType<WorldMock>("//meworla/world");
		server = std::make_unique<ServerServiceMock>("server", "//meworla/serverservice");
		app->registerService("server", server.get());

		// setup message stream
		Psio::TCPSocket* tcp_sock = new Psio::TCPSocket(nullptr);
		std::unique_ptr<Psio::TCPSocket> sock;
		sock.reset(tcp_sock);
		
		hermes = std::make_unique<HermesServerMock>(std::move(sock), nullptr);

		// user mgmt
		static bool initialized = false;
		if (!initialized) {
			AccountService::RegisterAccountFabrics(app);
			initialized = true;
		}

		acc_srv = std::make_unique<AccountService>("accountservice", "//meworla/accountservice");
		app->registerService("accounts", acc_srv.get());
		std::unique_ptr<ProviderMemory> provider = std::make_unique<ProviderMemory>();
		acc_srv->setProvider(std::move(provider));
	}

	void TearDown() override {		
		hermes.reset();
		delete scene;
		delete app;
	}
};

MATCHER_P(MsgEq, value, "") { return (arg->base.opcode == value); }

// Check if Start calls onLogin of hermes
TEST_F(ClientConnectionTest, StartTest)
{
	HermesServerMock* hermes_ref = hermes.get();
	ASSERT_TRUE(hermes_ref != nullptr);
		
	// Define variables to capture callback arguments
	std::unique_ptr<Hermes::Message> capturedMessage;
	int capturedError = -1;

	EXPECT_CALL(*hermes_ref, onLogin)
		.Times(1).
		WillOnce(
			testing::DoAll(
				//testing::InvokeArgument<0>, // not for Login test (only ttest if handler gets called)
				testing::Return(0)
			)
		);	

	ClientConnection conn(app, scene, std::move(hermes));

	testing::MockFunction<void(int err)> mock_handler;

	int ret = conn.Start(mock_handler.AsStdFunction());
}

// Check if Start calls onLoginComplete of hermes
// if Login message was received
// expect auth failure - no accounts are set
TEST_F(ClientConnectionTest, StartTestLoginCompleted)
{
	HermesServerMock* hermes_ref = hermes.get();
	ASSERT_TRUE(hermes_ref != nullptr);

	WorldMock world("testworld", "//meworla/world");
	app->getUnitTree()->addChild(&world);

	// Define variables to capture callback arguments
	std::unique_ptr<Hermes::Message> capturedMessage;
	int capturedError = -1;

	EXPECT_CALL(*hermes_ref, onLogin)
		.Times(1)
		.WillOnce(
			testing::DoAll(
				testing::InvokeArgument<0>("anonymous", 0),
				testing::Return(0)
			)
		);

	EXPECT_CALL(*hermes_ref, sendMessage(MsgEq(HERMES_MSG_LOGIN_FAILED), _))
		.Times(1)
		.WillOnce(
			testing::DoAll(
				testing::InvokeArgument<1>(1),
				testing::Return(0)
			)
		);

	ClientConnection conn(app, scene, std::move(hermes));

	testing::MockFunction<void(int err)> mock_handler;

	EXPECT_CALL(mock_handler, Call(_))
		.Times(1);

	int ret = conn.Start(mock_handler.AsStdFunction());
	
}

// Check if Start calls onLoginComplete of hermes
// and send an auth success mesage back
TEST_F(ClientConnectionTest, LoginCompletedSendOK)
{
	HermesServerMock* hermes_ref = hermes.get();
	ASSERT_TRUE(hermes_ref != nullptr);

	WorldMock world("testworld", "//meworla/world");
	app->getUnitTree()->addChild(&world);
	User user_info("anonymous", "//meworla/user");
	((ProviderMemory*)acc_srv->getProvider())->addUser(&user_info);

	EXPECT_CALL(*hermes_ref, onLogin)
		.Times(1)
		.WillOnce(
			testing::DoAll(
				testing::InvokeArgument<0>("anonymous", 0),
				testing::Return(0)
			)
		);

	EXPECT_CALL(*hermes_ref, sendMessage(MsgEq(HERMES_MSG_LOGIN_SUCCEEDED),_))
		.Times(1);

	EXPECT_CALL(*hermes_ref, onMessage)
		.Times(1)
		.WillOnce(
			testing::Return(-1)
		);

	ClientConnection conn(app, scene, std::move(hermes));

	testing::MockFunction<void(int err)> mock_handler;

	int ret = conn.Start(mock_handler.AsStdFunction());
}

// Check if Start calls onLoginComplete of hermes
// and send auth error for not existing user
TEST_F(ClientConnectionTest, LoginCompletedUnknownUser)
{
	HermesServerMock* hermes_ref = hermes.get();
	ASSERT_TRUE(hermes_ref != nullptr);

	WorldMock world("testworld", "//meworla/world");
	app->getUnitTree()->addChild(&world);
	User user_info("nouser", "//meworla/user");
	((ProviderMemory*)acc_srv->getProvider())->addUser(&user_info);

	EXPECT_CALL(*hermes_ref, onLogin)
		.Times(1)
		.WillOnce(
			testing::DoAll(
				testing::InvokeArgument<0>("anonymous", 0),
				testing::Return(0)
			)
		);

	EXPECT_CALL(*hermes_ref, sendMessage(MsgEq(HERMES_MSG_LOGIN_FAILED), _))
		.Times(1)
		.WillOnce(
			testing::DoAll(
				testing::InvokeArgument<1>(1),
				testing::Return(0)
			)
		);
		
	EXPECT_CALL(*hermes_ref, onMessage)
		.Times(0);

	ClientConnection conn(app, scene, std::move(hermes));

	testing::MockFunction<void(int err)> mock_handler;

	EXPECT_CALL(mock_handler, Call(_))
		.Times(1);

	int ret = conn.Start(mock_handler.AsStdFunction());
}

// test if after login an avatar is sent
// the avatar does not exist -> expect failed message
TEST_F(ClientConnectionTest, OnAvatarNotExisting)
{
	HermesServerMock* hermes_ref = hermes.get();
	ASSERT_TRUE(hermes_ref != nullptr);

	WorldMock world("testworld", "//meworla/world");
	app->getUnitTree()->addChild(&world);
	User user_info("anonymous", "//meworla/user");
	((ProviderMemory*)acc_srv->getProvider())->addUser(&user_info);

	EXPECT_CALL(*hermes_ref, sendMessage(MsgEq(HERMES_MSG_ENTER_FAILED), _))
		.Times(1)
		.WillOnce(
			testing::DoAll(
				testing::Return(-1)
			)
		);

	EXPECT_CALL(*hermes_ref, onMessage)
		.Times(1)
		.WillOnce(
			testing::DoAll(
				testing::WithArg<0>([](Hermes::ServerStream::MessageHandler h) {
					std::unique_ptr<Hermes::MessageEnterWorld> msg = std::make_unique<Hermes::MessageEnterWorld>();
					msg->avatar = "avatar";
					h(std::move(msg), 0);
					}),
				testing::Return(-1)
			)
		);

	

	ClientConnection conn(app, scene, std::move(hermes));

	testing::MockFunction<void(int err)> mock_handler;

	conn.StartConnectedUser(&user_info);
}

// test if after login an avatar is sent
// the avatar does exist -> expect adding to world 
// and information of all other users
TEST_F(ClientConnectionTest, OnAvatarSucess)
{
	HermesServerMock* hermes_ref = hermes.get();
	ASSERT_TRUE(hermes_ref != nullptr);

	WorldMock world("testworld", "//meworla/world");
	app->getUnitTree()->addChild(&world);
	User user_info("anonymous", "//meworla/user");	
	Rhea::Avatar* avatar = new Rhea::Avatar("avatar", "//meworla/avatar");
	avatar->setValue("model", "//avatar_model");	
	user_info.addChild(avatar);
	Rhea::Item* avatar_model = new Rhea::Item("avatar_model", "//meworla/item");
	((ProviderMemory*)acc_srv->getProvider())->addUser(&user_info);

	app->getUnitTree()->addChild(avatar_model);

	// Define variables to capture callback arguments
	Hermes::Message* capturedMessage = nullptr;
	int capturedError = -1;

	EXPECT_CALL(*hermes_ref, sendMessage(MsgEq(HERMES_MSG_CREATE), _))
		.Times(1)
		.WillOnce(
			testing::DoAll(
				testing::Return(-1)
			)
		);

	EXPECT_CALL(*hermes_ref, onMessage)
		.Times(2)
		.WillOnce(
			testing::DoAll(
				testing::WithArg<0>([](Hermes::ServerStream::MessageHandler h) {
					std::unique_ptr<Hermes::MessageEnterWorld> msg = std::make_unique<Hermes::MessageEnterWorld>();
					msg->avatar = "avatar";
					h(std::move(msg), 0);
					}),
				testing::Return(-1)
			)
		)
		.WillRepeatedly(
			testing::Return(-1)
		);

	ServerServiceMock* server_ref = server.get();
	EXPECT_CALL(*server_ref, broadcast)
		.Times(1);

	ClientConnection conn(app, scene, std::move(hermes));

	testing::MockFunction<void(int err)> mock_handler;

	conn.StartConnectedUser(&user_info);
}


// Check if move message calls player.setMovement
TEST_F(ClientConnectionTest, CheckMoveMessage)
{
	HermesServerMock* hermes_ref = hermes.get();
	ASSERT_TRUE(hermes_ref != nullptr);

	player = std::make_unique<PlayerMock>("player", "//meworla/item");
	PlayerMock* player_ref = player.get();

	EXPECT_CALL(*hermes_ref, onMessage)
		.Times(1)
		.WillOnce(
			testing::DoAll(
				testing::WithArg<0>([](Hermes::ServerStream::MessageHandler h) {
					std::unique_ptr<Hermes::MessageMove> msg = std::make_unique<Hermes::MessageMove>();
					msg->move[0] = 1.0f;
					msg->move[1] = 1.0f;
					msg->move[2] = 1.0f;
					h(std::move(msg), 0);
					}),
				testing::Return(0)
			)
		);

	EXPECT_CALL(*player_ref, setMovement)
		.Times(1);

	ClientConnection conn(app, scene, std::move(hermes));

	// start in the established connection
	// this should set handler
	conn.StartEstablishedConnection(player_ref);
}

// Check if move message calls player.setTurn
TEST_F(ClientConnectionTest, CheckTurnMessage)
{
	HermesServerMock* hermes_ref = hermes.get();
	ASSERT_TRUE(hermes_ref != nullptr);

	player = std::make_unique<PlayerMock>("player", "//meworla/item");
	PlayerMock* player_ref = player.get();

	EXPECT_CALL(*hermes_ref, onMessage)
		.Times(1)
		.WillOnce(
			testing::DoAll(
				testing::WithArg<0>([](Hermes::ServerStream::MessageHandler h) {
					std::unique_ptr<Hermes::MessageTurn> msg = std::make_unique<Hermes::MessageTurn>();
					msg->turn = 1.0f;
					h(std::move(msg), 0);
					}),
				testing::Return(0)
			)
		);

	EXPECT_CALL(*player_ref, setTurn)
		.Times(1);

	ClientConnection conn(app, scene, std::move(hermes));

	// start in the established connection
	// this should set handler
	conn.StartEstablishedConnection(player_ref);
}