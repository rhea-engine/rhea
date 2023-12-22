#include "ClientConnection.h"
#include <Rhea/UnitTree.h>
#include <Rhea/Fabrics/3D/Item.h>
#include <Rhea/Physics/PhysicsService.h>
#include <Hermes/Messages/MessageCreate.h>
#include <Hermes/Messages/MessageMove.h>
#include <Hermes/Messages/MessageTurn.h>
#include <Hermes/Messages/MessagePosition.h>
#include <Hermes/Messages/MessageAvatarList.h>
#include <Hermes/Messages/MessageEnterWorld.h>
#include <Rhea/Fabrics/Core/Avatar.h>
#include <Psio/Log.h>
#include "ServerService.h"
#include <Hermes/Messages/MessageEnterFailed.h>
#include <Hermes/Messages/MessageLogin.h>
#include "Accounts/AccountService.h"
#include "Authorization/AuthorizationService.h"
#include <Hermes/Messages/MessageCreateAvatar.h>

int ClientConnection::Start(Psio::Network::CompletionHandler err_handler)
{
	m_error_handler = err_handler;

	Hermes::MessageFactory* factory = m_hermes->getMessageFactory();
	if (!factory) {
		LOG(LOG_ERROR) << "No message factory found";
		return -2;
	}

	// auth state
	m_hermes->onLogin([&](std::string method, std::string username, std::vector<std::string> secrets) {
		Login(method, username, secrets);
		});

	m_hermes->onCreateUser(std::bind(&ClientConnection::CreateUser, this, 
		std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5));

	// connection established

	// lobby

	factory->registerMessageHandler<Hermes::Message>(HERMES_MSG_GET_AVATARLIST,
		[&](uint16_t opcode)->bool {
			return m_state == CONN_LOBBY;
		},
		std::bind(&ClientConnection::onGetAvatarListMessage, this, std::placeholders::_1)
	);

	factory->registerMessageHandler<Hermes::MessageCreateAvatar>(HERMES_MSG_CREATEAVATAR,
		[&](uint16_t opcode)->bool {
			return m_state == CONN_LOBBY;
		},
		std::bind(&ClientConnection::onCreateAvatarMessage, this, std::placeholders::_1)
	);
	
	// world messages

	factory->registerMessageHandler<Hermes::MessageEnterWorld>(HERMES_MSG_ENTERWORLD,
		[&](uint16_t opcode)->bool {
			return m_state == CONN_LOBBY;
		},
		std::bind(&ClientConnection::onEnterWorldMessage, this, std::placeholders::_1)
	);

	factory->registerMessageHandler<Hermes::MessageMove>(HERMES_MSG_MOVE,
		[&](uint16_t opcode)->bool {
			return m_state == CONN_IN_WORLD;
		},
		std::bind(&ClientConnection::onMoveMessage, this, std::placeholders::_1)
	);

	factory->registerMessageHandler<Hermes::MessageTurn>(HERMES_MSG_TURN,
		[&](uint16_t opcode)->bool {
			return m_state == CONN_IN_WORLD;
		},
		std::bind(&ClientConnection::onTurnMessage, this, std::placeholders::_1)
	);

	// error handling

	m_hermes->onClosed([&](int err) {
		m_hermes.reset(nullptr);
		});

	// start the connection - wait for login	
	m_state = CONN_LOGIN;
	return m_hermes->Connect();
}

int ClientConnection::sendMessage(Hermes::Message* msg)
{
	// forward to stream
	return m_hermes->sendMessage(msg, [&](int err) {
		if (err < -1) {
			m_state = CONN_CLOSE;
			m_hermes->Close();
			if (m_error_handler)
				m_error_handler(err);
		}
		});
}

void ClientConnection::Close()
{
	m_state = CONN_CLOSE;
	m_hermes->Close();
}

void ClientConnection::CreateUser(std::string method, std::string username, std::string name, std::string ext_id, std::vector<std::string> secrets)
{
	AuthorizationService* auth_srv = doCast<AuthorizationService>(m_app->getService("authorization"));
	if (!auth_srv) {
		LOG(LOG_ERROR) << "Configuration error: Authorization service not found";
		// Send auth failure
		m_state = CONN_LOGIN;
		m_hermes->AuthFailure([&](int result) {
			m_hermes->LoginRetry();
			},
			std::vector<std::string>());
		return;
	}
	UserInfo user_info;
	user_info.username = username;
	user_info.user_id = "";
	if (name != "")
		user_info.name = name;
	else
		user_info.name = username;
	user_info.ext_id = ext_id;
	auth_srv->CreateUser(method, user_info, secrets, [&, user_info, secrets](std::string user_id, int err) {
		if (!err) {
			// success

			// create the account for the user
			AccountService* acc_srv = doCast<AccountService>(m_app->getService("accounts"));
			if (!acc_srv) {
				LOG(LOG_ERROR) << "Configuration error: Account service not found";
				// Send auth failure
				m_state = CONN_LOGIN;
				m_hermes->AuthFailure([&](int result) {
					m_hermes->LoginRetry();
					},
					std::vector<std::string>());
				return;
			}

			// log the user in
			Login(method, user_info.username, secrets);
		}
		else {
			// Send auth failure
			m_state = CONN_LOGIN;
			m_hermes->AuthFailure([&](int result) {
				m_hermes->LoginRetry();
				},
				std::vector<std::string>());
		}
		});
}

void ClientConnection::Login(std::string method, std::string username, std::vector<std::string> secrets) {
	
	// login message received - try to log the user in
	m_world = (World*)m_app->getUnitTree()->findByType("//meworla/world");
	if (m_world) {
		m_username = username;
		AuthorizationService* auth_srv = doCast<AuthorizationService>(m_app->getService("authorization"));
		if (!auth_srv) {
			LOG(LOG_ERROR) << "Configuration error: Authorization service not found";
			// Send auth failure
			m_state = CONN_LOGIN;
			m_hermes->AuthFailure([&](int result) {
				m_hermes->LoginRetry();
				},
				std::vector<std::string>());
			return;
		}
		auth_srv->getUserId(method, m_username, secrets, [&](AuthInfo auth_info) {

			if (auth_info.user_id == "") {
				LOG(LOG_ERROR) << "User " << m_username << " is not registered";
				// Send auth failure
				m_state = CONN_LOGIN;
				m_hermes->AuthFailure([&](int result) {
					m_hermes->LoginRetry();
					},
					auth_srv->getMethods());
				return;
			}
			AccountService* acc_srv = doCast<AccountService>(m_app->getService("accounts"));
			if (!acc_srv) {
				LOG(LOG_ERROR) << "Configuration error: Account service not found";
				m_user = nullptr;
			}
			else {
				// get the user
				acc_srv->GetAccount(auth_info, [&](User* user, int err) {
					if (!err && user) {
						m_user = user;
						// login succesful - send auth succcess
						m_state = CONN_LOBBY;
						m_hermes->AuthSuccess([&](int result) {
							if (result < -1 && m_error_handler)
								m_error_handler(result);
							});
					}
					else {
						LOG(LOG_ERROR) << "User with id " << auth_info.user_id << " is not registered and could not be created.";
						// Send auth failure
						m_state = CONN_LOGIN;
						m_hermes->AuthFailure([&](int result) {
							m_hermes->LoginRetry();
							},
							auth_srv->getMethods());
					}
					});
			}
			});
	}
	else {
		LOG(LOG_ERROR) << "Configuration error: World not found";
	}
}

void ClientConnection::onGetAvatarListMessage(Hermes::Message* msg)
{
	// send avatar list (which may be empty)

	// fill the avatars in the avatarlist message
	Hermes::MessageAvatarList msg_al;
	m_user->Avatars([&msg_al](Rhea::Avatar* avatar)->bool {
		std::string name = avatar->getPathName();
		std::string model = avatar->getValue("model");
		msg_al.addAvatar(name, model);
		return true;
		});

	m_state = CONN_LOBBY;
	m_hermes->sendMessage(&msg_al, [&](int err) {
		if (err <0 && m_error_handler)
			m_error_handler(err);
	});
}

void ClientConnection::onCreateAvatarMessage(Hermes::MessageCreateAvatar* msg_avatar)
{
	Math::Vector3 position = { 128.0f, 0.0f, 10.0f }; // default position
	AccountService* acc_srv;
	Rhea::Avatar* avatar;

	acc_srv = doCast<AccountService>(m_app->getService("accounts"));
	avatar = new Rhea::Avatar(msg_avatar->avatar.name, "//meworla/avatar");
	avatar->setModel(msg_avatar->avatar.type_name);
	avatar->setPosition(position);
	acc_srv->CreateAvatar(m_user, avatar, [&, avatar](int err) {
		if (!err) {
			// enter the world with this avatar
			return EnterWorld(avatar);
		}
		else {
			// avatar could not be added - delete it
			delete avatar;
			Hermes::MessageEnterFailed errmsg(HERMES_MSG_CREATEAVATAR_FAILED);
			errmsg.reason = err;
			m_state = CONN_LOBBY;
			return m_hermes->sendMessage((Hermes::Message*)&errmsg, [&](int err) {
				if (err < -1 && m_error_handler)
					m_error_handler(err);
				});
		}
		});
}

void ClientConnection::onEnterWorldMessage(Hermes::MessageEnterWorld* msg)
{
	// get the avatar with that name
	Rhea::Avatar* avatar = nullptr;
	if (m_user) {
		avatar = m_user->getAvatar(msg->avatar);
	}
	if (!avatar) {
		LOG(LOG_ERROR) << "Avatar " << msg->avatar << " not found";
		// send error message
		Hermes::MessageEnterFailed errmsg;
		errmsg.reason = 1;
		m_state = CONN_LOBBY;
		m_hermes->sendMessage((Hermes::Message*)&errmsg, [&](int err) {
			if (err < 0 && m_error_handler)
				m_error_handler(err);
			});
	}

	EnterWorld(avatar);
}

void ClientConnection::onMoveMessage(Hermes::MessageMove* move_msg)
{
	m_player->setMovement(Math::Vector3(move_msg->move[0], move_msg->move[1], move_msg->move[2]));
	DLOG(LOG_DEBUG) << "MoveMsg received: " << move_msg->move[0] << ", " << move_msg->move[1] << ", " << move_msg->move[2];
}

void ClientConnection::onTurnMessage(Hermes::MessageTurn* turn_msg)
{
	m_player->setTurn(turn_msg->turn);
	DLOG(LOG_DEBUG) << "TurnMsg received: " << turn_msg->turn;
}

int ClientConnection::EnterWorld(Rhea::Avatar* avatar)
{
	// instancing the player
	Rhea::Item* player_model = (Rhea::Item*)m_app->getUnitTree()->find(avatar->getValue("model"));
	if (!player_model) {
		LOG(LOG_ERROR) << " could not open avatar model " << avatar->getModel();
		// send error message
		Hermes::MessageEnterFailed errmsg;
		errmsg.reason = 2;
		return m_hermes->sendMessage((Hermes::Message*)&errmsg, [&](int err) {
			if (err < 0 && m_error_handler)
				m_error_handler(err);
			});
	}

	std::string avatar_name = avatar->getPathName();

	LOG(LOG_INFO) << "Avatar " << avatar_name << " entered scene";

	int i = 0;
	std::string internal_name = avatar_name;
	while (m_scene->find(internal_name)) {
		// make sure no items with the same name exist
		internal_name = avatar_name + std::string("-") + std::to_string(++i);
	}
	m_player = player_model->instance(internal_name);
	Math::Vector3 position = { 128.0f, 0.0f, 10.0f }; // default position
	position = avatar->getPosition();
	m_scene->addChild(m_player);
	m_player->setPosition(position);
	m_player->setValue("avatar", avatar);

	// add a network controller
	ServerService* server_srv = (ServerService*)m_app->getService("server");
	int id = server_srv->getUID();
	std::unique_ptr<Rhea::NetworkComponent> net_com = std::make_unique<Rhea::NetworkComponent>(m_player, id, NET_ROLE_SERVER);
	m_player->setNetworkComponent(std::move(net_com));

	m_scene->addItem(m_player);

	m_player->ready();
	DLOG(LOG_DEBUG) << "player ready";

	Rhea::Service* phy_srv = m_app->getService("physics");
	// add to physics					
	if (phy_srv) {
		((Rhea::PhysicsService*)phy_srv)->initializeItem(m_player);
	}

	// inform connector
	Hermes::MessageCreate msg;
	Math::Vector3 pos = m_player->getTransform()->get_position()->get_worldspace();
	msg.path = internal_name;
	msg.type_name = avatar->getValue("model");
	msg.NUID = m_player->getNetworkComponent()->NUID();
	msg.pos[0] = pos.x;
	msg.pos[1] = pos.y;
	msg.pos[2] = pos.z;
	Math::Quaternion q = m_player->getTransform()->get_position()->get_rotation();
	msg.transform[0] = q.w;
	msg.transform[1] = q.x;
	msg.transform[2] = q.y;
	msg.transform[3] = q.z;
	msg.is_player = true;
	m_hermes->sendMessage((Hermes::Message*)&msg, nullptr);

	// send all items with network component
	std::vector<Rhea::Item*> items = m_scene->getItems();
	for (auto item_it = items.begin(); item_it != items.end(); ++item_it) {
		if (*item_it == m_player)
			continue;
		Rhea::NetworkComponent* net_com = (*item_it)->getNetworkComponent();
		if (net_com) {
			Hermes::MessageCreate item_msg;
			Math::Vector3 pos = (*item_it)->getTransform()->get_position()->get_worldspace();
			item_msg.path = (*item_it)->getPathName();
			avatar = (Rhea::Avatar*)(Variant*)(*item_it)->getValue("avatar");
			if (avatar) {
				item_msg.type_name = avatar->getValue("model");
			}
			else {
				// no avatar -> TODO: check can there be normal items without avatar?
				continue;
			}
			item_msg.NUID = (*item_it)->getNetworkComponent()->NUID();
			item_msg.pos[0] = pos.x;
			item_msg.pos[1] = pos.y;
			item_msg.pos[2] = pos.z;
			Math::Quaternion q = (*item_it)->getTransform()->get_position()->get_rotation();
			item_msg.transform[0] = q.w;
			item_msg.transform[1] = q.x;
			item_msg.transform[2] = q.y;
			item_msg.transform[3] = q.z;
			item_msg.is_player = false;
			m_hermes->sendMessage((Hermes::Message*)&item_msg, nullptr);
		}
	}
	// inform all other connectors
	msg.is_player = false;
	server_srv->broadcast((Hermes::Message*)&msg, this);

	// enter normal state
	m_state = CONN_IN_WORLD;	
		
	return 0;
}

void ClientConnection::StartConnectedUser(User* user)
{
	m_user = user;
	m_state = CONN_LOBBY;
}

// helper for testing
void ClientConnection::StartEstablishedConnection(Rhea::Item* player)
{
	m_player = player;
	m_state = CONN_IN_WORLD;
}
