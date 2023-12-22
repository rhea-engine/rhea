#include "WorldClient.h"
#include <Hermes/Opcode.h>
#include <Rhea/UnitTree.h>
#include <Rhea/Fabrics/3D/Item.h>
#include <Psio/Log.h>
#include <Hermes/Messages/MessageMove.h>
#include <Hermes/Messages/MessageTurn.h>
#include <Hermes/Messages/MessagePosition.h>
#include <Hermes/Messages/MessageCreate.h>
#include <Hermes/Messages/MessageDestroy.h>
#include <Hermes/Messages/MessageAvatarList.h>
#include <Rhea/Fabrics/Core/Avatar.h>
#include <Hermes/Messages/MessageEnterWorld.h>
#include <Application.h>
#include "ClientService.h"

namespace Rhea
{
	void WorldClient::setAuth(std::string method, std::string username, std::vector<std::string> secrets)
	{
		m_method = method;
		m_username = username;
		m_secrets = secrets;
	}

	void WorldClient::Open(std::string worldname)
	{
		Rhea::ClientService* service = doCast<ClientService>(m_app->getService("hermesclient"));
		if (!service) {
			LOG(LOG_ERROR) << "Configuration error: no hermesclient service defined";
			m_values["error"] = 1;
			Event ev("error", this);
			sendEvent("error", &ev);
			return;
		}

		service->Connect(worldname, [&, service](Hermes::ClientStream* stream, int err) {
			if (!err) {
				service->addChild(this);
				m_hermes.reset(stream);
				Event ev("connected", this);
				sendEvent("connected", &ev);
			}
			else {
				m_values["error"] = err;
				Event ev("error", this);
				sendEvent("error", &ev);
			}
			});
	}

	void WorldClient::Start()
	{
		Hermes::MessageFactory* factory = m_hermes->getMessageFactory();
		if (!factory) {
			LOG(LOG_ERROR) << "No message factory found";
			return;
		}

		factory->registerMessageHandler<Hermes::MessageAvatarList>(HERMES_MSG_AVATARLIST,
			[&](uint16_t opcode)->bool {
				return !m_in_world;
			},
			std::bind(&WorldClient::onAvatarlistMessage, this, std::placeholders::_1)
		);

		factory->registerMessageHandler<Hermes::Message>(HERMES_MSG_CREATEAVATAR_FAILED,
			[&](uint16_t opcode)->bool {
				return !m_in_world;
			},
			std::bind(&WorldClient::onCreateAvatarFailedMessage, this, std::placeholders::_1)
		);

		factory->registerMessageHandler<Hermes::Message>(HERMES_MSG_ENTER_FAILED,
			[&](uint16_t opcode)->bool {
				return m_in_world;
			},
			std::bind(&WorldClient::onEnterWorldFailedMessage, this, std::placeholders::_1)
		);

		factory->registerMessageHandler<Hermes::MessageCreate>(HERMES_MSG_CREATE,
			[&](uint16_t opcode)->bool {
				return m_in_world;
			},
			std::bind(&WorldClient::onCreateMessage, this, std::placeholders::_1)
		);

		factory->registerMessageHandler<Hermes::MessageDestroy>(HERMES_MSG_DESTROY,
			[&](uint16_t opcode)->bool {
				return m_in_world;
			},
			std::bind(&WorldClient::onDestroyMessage, this, std::placeholders::_1)
		);


		factory->registerMessageHandler<Hermes::MessagePosition>(HERMES_MSG_POSITION,
			[&](uint16_t opcode)->bool {
				return m_in_world;
			},
			std::bind(&WorldClient::onPositionMessage , this, std::placeholders::_1)
		);

		if (m_username.empty()) {
			LOG(LOG_ERROR) << "WorldClient: user name is empty";
			Event ev("login_failure", this);
			sendEvent("login_failure", &ev);
			return;
		}

		// start with Login
		m_hermes->Login(m_method, m_username, m_secrets, [&](int err) {
			if (err != 0) {
				// handle login failure
				DLOG(LOG_DEBUG) << "login failed " << err;
				Event ev("login_failure", this);
				sendEvent("login_failure", &ev);
			}
			else {
				onLogin();
			}
			});
	}

	void WorldClient::Register(std::string method, std::string username, std::string name, std::string ext_id, std::vector<std::string> secrets)
	{
		if (username.empty()) {
			LOG(LOG_ERROR) << "WorldClient: user name is empty";
			Event ev("login_failure", this);
			sendEvent("login_failure", &ev);
			return;
		}
		m_hermes->Register(m_method, username, name, ext_id, m_secrets, [&](int err) {
			if (err && err != HERMES_MSG_LOGIN_SUCCEEDED) {
				// handle login failure
				DLOG(LOG_DEBUG) << "register failed " << err;
				Event ev("register_failure", this);
				sendEvent("register_failure", &ev);
			}
			else {
				onLogin();
			}
			});
	}

	void WorldClient::CreateAvatar(std::string name, std::string type_name)
	{
		m_hermes->CreateAvatar(name, type_name);
	}

	void WorldClient::onLogin() {

		DLOG(LOG_DEBUG) << "login sent";

		// if this world uses avatars get the current list
		Hermes::Message msg_get_avatar_list(HERMES_MSG_GET_AVATARLIST);
		m_hermes->sendMessage(&msg_get_avatar_list, nullptr);

		m_hermes->Loop();
	}
	
	void WorldClient::onAvatarlistMessage(Hermes::MessageAvatarList*msg)
	{
		// fill up avatar list of scene
		Rhea::Unit* avatar_list = getChild("avatarlist");
		if (!avatar_list) {
			avatar_list = new Rhea::Unit("avatarlist", "//meworla/avatarlist");
			avatar_list->setup(m_app);
			addChild(avatar_list);
		}
		if (msg->avatars.size()) {
			for (auto it = msg->avatars.begin(); it != msg->avatars.end(); it++) {
				Rhea::Avatar* avatar = (Rhea::Avatar*)m_app->getUnitTree()->createUnit((*it).avatar, "//meworla/avatar");
				avatar->setModel((*it).type_name);
				avatar_list->addChild(avatar);
			}
		}
		m_values["nofAvatars"] = (int)msg->avatars.size();
		Event ev("avatarlist", this);
		sendEvent("avatarlist", &ev);
	}

	void WorldClient::onCreateAvatarFailedMessage(Hermes::Message* msg)
	{
		// avatar entering failed
		m_in_world = false;
		
		// inform user
		Event ev("create_avatar_failed", this);
		sendEvent("create_avatar_failed", &ev);
	}

	void WorldClient::onEnterWorldFailedMessage(Hermes::Message* msg)
	{
		// avatar entering failed
		m_in_world = false;

		// inform user
		Event ev("entering_failed", this);
		sendEvent("entering_failed", &ev);
	}

	void WorldClient::onCreateMessage(Hermes::MessageCreate* msg)
	{
		// instancing the player
		Rhea::Item* gnar = doCast<Rhea::Item>(m_app->getUnitTree()->find(msg->type_name));
		if (gnar) {
			Rhea::Item* player;
			if (msg->is_player && m_player_set) {
				LOG(LOG_ERROR) << "isPlayer set multiple times";
				msg->is_player = false;
			}
			if (msg->is_player) {
				player = gnar->instance("player");
				m_player_set = true;
			}
			else
				player = gnar->instance(msg->path);

			m_scene->addChild(player);
			Math::Vector3 position = Math::Vector3(msg->pos[0], msg->pos[1], msg->pos[2]);
			Math::Quaternion rot = Math::Quaternion(msg->transform[0], msg->transform[1], msg->transform[2], msg->transform[3]);
			player->setPosition(position);
			player->getTransform()->get_position()->setOrientation(rot);
			std::unique_ptr<Rhea::NetworkComponent> net_comp = std::make_unique<Rhea::NetworkComponent>(player, msg->NUID, NET_ROLE_CLIENT);
			net_comp->setDelegate(this);
			player->setNetworkComponent(std::move(net_comp));
			m_net_items[msg->NUID] = player;
			Rhea::Event e("scene_enter", player);
			m_scene->bubbleEvent("scene_enter", &e);

			m_scene->addItem(player);
			player->ready();

			if (msg->is_player) {
				Event ev("world_enter", this);
				sendEvent("world_enter", &ev);
			}
		}
		else {
			LOG(LOG_ERROR) << "Did not find player template";
		}
	}
	
	void WorldClient::onPositionMessage(Hermes::MessagePosition* move_msg)
	{
		Rhea::Position pos;
		pos.setPosition(move_msg->pos[0], move_msg->pos[1], move_msg->pos[2]);
		Math::Quaternion q(move_msg->transform[0], move_msg->transform[1], move_msg->transform[2], move_msg->transform[3]);
		pos.setOrientation(q);
		auto it = m_net_items.find(move_msg->NUID);
		if (it != m_net_items.end()) {
			it->second->setPosition(pos);
		}
		else {
			DLOG(LOG_DEBUG) << "Net item with id " << move_msg->NUID << " not set";
		}
		DLOG(LOG_DEBUG) << "Position received " << move_msg->pos[0] << ", " << move_msg->pos[1] << ", " << move_msg->pos[2] << " for[" << move_msg->NUID << "]";
	}

	void WorldClient::onDestroyMessage(Hermes::MessageDestroy* msg)
	{
		// destruction of item requested
		auto it = m_net_items.find(msg->NUID);
		if (it != m_net_items.end()) {
			Rhea::Item* item = it->second;
			if (m_scene) {
				m_scene->removeItem(item);
				m_scene->removeChild(item);
			}
			delete item;
			m_net_items.erase(it);
		}
		// Note if not found we ignore it
	}

	void WorldClient::stop()
	{
		m_hermes.reset();
	}

	void WorldClient::enterWorld(std::string avatar_name)
	{
		// send enter with the given avatar name
		Hermes::MessageEnterWorld msg_enter;
		msg_enter.avatar = avatar_name;
		m_in_world = true;
		m_hermes->sendMessage(&msg_enter, nullptr);
	}

	void WorldClient::changeMovement(int nuid, Math::Vector3 move)
	{
		Hermes::MessageMove move_msg;
		move_msg.NUID = nuid;
		move_msg.move[0] = move.x;
		move_msg.move[1] = move.y;
		move_msg.move[2] = move.z;

		m_hermes->sendMessage(&move_msg, nullptr);
	}

	void WorldClient::changeTurn(int nuid, float turn)
	{
		Hermes::MessageTurn turn_msg;
		turn_msg.NUID = nuid;
		turn_msg.turn = turn;

		m_hermes->sendMessage(&turn_msg, nullptr);
	}
}