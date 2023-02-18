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

int ClientConnection::Start(Psio::Network::CompletionHandler err_handler)
{
	// start the connection - wait for login
	m_error_handler = err_handler;
	m_next_state = CONN_LOGIN;
	return DoLoop(0);
}

int ClientConnection::sendMessage(Hermes::Message* msg)
{
	if (m_state == CONN_MESSAGE) {
		int ret = m_hermes->sendMessage(msg, [&](int err) {
			if (err < 0) {
				m_state = CONN_CLOSE;
				m_hermes.reset(nullptr);
				m_error_handler(err);
			}
			});
		if (ret < -1) {
			m_state = CONN_CLOSE;
			m_hermes.reset(nullptr);
			DLOG(LOG_DEBUG) << "Connection: sendMessage failed: " << ret;
		}
		return ret;
	}
	return 0;
}

int ClientConnection::DoLoop(int result)
{
	do
	{
		m_state = m_next_state;
		m_next_state = CONN_NONE;

		switch (m_state) {
		case CONN_NONE:
			break;
		case CONN_LOGIN:
			result = DoLogin(result);
			break;
		case CONN_LOGIN_COMPLETED:
			result = DoLoginCompleted(result);
			break;
		case CONN_AVATAR:
			result = DoAvatar(result);
			break;
		case CONN_AVATAR_COMPLETED:
			result = DoAvatarCompleted(result);
			break;
		case CONN_MESSAGE:
			result = DoMessage(result);
			break;
		case CONN_MESSAGE_COMPLETED:
			result = DoMessageCompleted(result);
			break;
		case CONN_CLOSE:
			result = DoClose(result);
		}

	} while (result != Psio::Network::IO_PENDING && m_state != CONN_NONE);


	return result;
}

void ClientConnection::CallDoLoop(int result)
{
	int res = DoLoop(result);
	if ((res < 0) && (res != Psio::Network::Err::IO_PENDING) && (m_error_handler != NULL))
		m_error_handler(res);
}

int ClientConnection::DoLogin(int result)
{
	if (result < 0)
		return result;

	int ret = m_hermes->onLogin([&](std::unique_ptr<Hermes::Message> msg, int err) {
		{
			m_next_state = CONN_LOGIN_COMPLETED;
			DoLoop(err);
		}
		});
	return ret;
}

int ClientConnection::DoLoginCompleted(int result)
{
	if (result < 0)
		return result;

	// login succesful, send avatar list (which may be empty)
	Hermes::MessageAvatarList msg_al;
	Rhea::Avatar* avatar = (Rhea::Avatar*)m_app->getUnitTree()->findByType("//meworla/avatar");
	if (avatar) {
		std::string name = avatar->getPathName();
		std::string model = avatar->getValue("model");
		msg_al.addAvatar(name, model);
	}
	m_hermes->sendMessage(&msg_al, nullptr);
	m_next_state = CONN_AVATAR;
	return 0;
}

int ClientConnection::DoAvatar(int result)
{
	if (result < 0)
		return result;

	int ret = m_hermes->onMessage([&](std::unique_ptr<Hermes::Message> msg, int err) {
		if (err >= 0) {
			// check if avatar was sent
			if (msg->getOpcode() != HERMES_MSG_ENTERWORLD) {
				if (m_error_handler)
					m_error_handler(-2); // Protocol error
			}
			else {
				// add avatar to scene
				Hermes::MessageEnterWorld* msgc = (Hermes::MessageEnterWorld*)msg.get();

				// get the avatar with that name
				Rhea::Avatar* avatar = (Rhea::Avatar*)m_app->getUnitTree()->findByType("//meworla/avatar");
				if (msgc->avatar.compare(avatar->getPathName())) {
					LOG(LOG_ERROR) << "Avatar " << msgc->avatar << " not found";
					// send some message
					return;
				}

				// instancing the player
				Rhea::Item* gnar = (Rhea::Item*)m_app->getUnitTree()->find(avatar->getValue("model"));
				if (gnar) {
					int i = 0;
					std::string basename = msgc->avatar;
					while (m_scene->find(msgc->avatar)) {
						// make sure no items with the same name exist
						msgc->avatar = basename + std::string("-") + std::to_string(++i);
					}
					m_player = gnar->instance(msgc->avatar);
					Math::Vector3 position = { 128.0f, 0.0f, 10.0f };
					m_scene->addChild(m_player);
					m_player->setPosition(position);

					// add a network controller
					ServerService* server_srv = (ServerService*)m_app->getService("server");
					int id = server_srv->getUID();
					std::unique_ptr<Rhea::NetworkComponent> net_com = std::make_unique<Rhea::NetworkComponent>(m_player, id, NET_ROLE_SERVER);
					m_player->setNetworkComponent(std::move(net_com));

					m_player->prepare([&, server_srv, avatar](Rhea::Error* err) {
						if (!err) {
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
							msg.path = msgc->avatar;
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

							// send all items with networ component
							std::vector<Rhea::Item*> items = m_scene->getItems();
							for (auto item_it = items.begin(); item_it != items.end(); ++item_it) {
								if (*item_it == m_player)
									continue;
								Rhea::NetworkComponent* net_com = (*item_it)->getNetworkComponent();
								if (net_com) {
									Hermes::MessageCreate item_msg;
									Math::Vector3 pos = (*item_it)->getTransform()->get_position()->get_worldspace();
									item_msg.path = (*item_it)->getPathName();
									item_msg.type_name = "//local/scene1/gnar";
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
							m_next_state = CONN_MESSAGE;
							DoLoop(0);
						}
						else {
							LOG(LOG_ERROR) << "Could not prepare player";
						}
						});
				}
				else {
					LOG(LOG_ERROR) << " could not open avatar " << msgc->avatar;
				}
			}
		}
		else {
			m_next_state = CONN_CLOSE;
			DoLoop(err);
		}
		});
	return ret;
}

int ClientConnection::DoAvatarCompleted(int result)
{
	return 0;
}

int ClientConnection::DoMessage(int result)
{
	if (result < 0)
		return result;

	int ret = m_hermes->onMessage([&](std::unique_ptr<Hermes::Message> msg, int err) {
		if (!err) {
			switch (msg->base.opcode) {
			case HERMES_MSG_MOVE:
			{
				Hermes::MessageMove* move_msg = (Hermes::MessageMove*)(msg.get());
				m_player->setMovement(Math::Vector3(move_msg->move[0], move_msg->move[1], move_msg->move[2]));
				DLOG(LOG_DEBUG) << "MoveMsg received: " << move_msg->move[0] << ", " << move_msg->move[1] << ", " << move_msg->move[2];
				
			}
			break;
			case HERMES_MSG_TURN:
			{
				Hermes::MessageTurn* turn_msg = (Hermes::MessageTurn*)(msg.get());
				m_player->setTurn(turn_msg->turn);
				DLOG(LOG_DEBUG) << "TurnMsg received: " << turn_msg->turn;
			}
			break;
			}
			m_next_state = CONN_MESSAGE;
			DoLoop(0);
		}
		else {
			// call error handler - this could destroy |this|
			m_state = CONN_CLOSE;			
			m_error_handler(err);
			return;
		}
		});
	return ret;
}

int ClientConnection::DoMessageCompleted(int result)
{
	return 0;
}

int ClientConnection::DoClose(int result)
{
	return 0;
}
