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

namespace Rhea
{
	void WorldClient::setAuth(std::string avatar)
	{
		m_avatar = avatar;
	}

	void WorldClient::start()
	{
		m_hermes->login([&](int err) {
			if (err >= 0) {
				DLOG(LOG_DEBUG) << "login succeeded";

				m_hermes->onMessage([&](std::unique_ptr<Hermes::Message> msg, int err) {
					if (!err) {
						if (msg->base.opcode == HERMES_MSG_AVATARLIST) {
							Hermes::MessageAvatarList* msgc = (Hermes::MessageAvatarList*)msg.get();
							if (msgc->avatars.size()) {

								// fill up avatar list of scene
								Rhea::Unit* avatar_list = getChild("avatarlist");
								if (!avatar_list) {
									avatar_list = new Rhea::Unit("avatarlist", "//meworla/avatarlist");
									avatar_list->setup(m_app);
									addChild(avatar_list);
								}
								for (auto it = msgc->avatars.begin(); it != msgc->avatars.end(); it++) {
									Rhea::Avatar* avatar = (Rhea::Avatar*)m_app->getUnitTree()->createUnit((*it).avatar, "//meworla/avatar");
									avatar->setModel((*it).type_name);
									avatar_list->addChild(avatar);
								}
							}
							m_values["nofAvatars"] = (int)msgc->avatars.size();
							Event ev("avatarlist", this);
							sendEvent("avatarlist", &ev);
						}
						else if (msg->base.opcode == HERMES_MSG_ENTER_FAILED) {
							// avatar entering failed
							// inform user
							Event ev("entering_failed", this);
							sendEvent("entering_failed", &ev);
							return;
						}
						else if (msg->base.opcode == HERMES_MSG_CREATE) {
							if (!m_scene) {
								LOG(LOG_ERROR) << "Network Client: Scene is not set";
							}
							else {
								Hermes::MessageCreate* msgc = (Hermes::MessageCreate*)msg.get();
								// instancing the player
								Rhea::Item* gnar = doCast<Rhea::Item>(m_app->getUnitTree()->find(msgc->type_name));
								if (gnar) {
									Rhea::Item* player;
									if (msgc->is_player && m_player_set) {
										LOG(LOG_ERROR) << "isPlayer set multiple times";
										msgc->is_player = false;
									}
									if (msgc->is_player) {
										player = gnar->instance("player");
										m_player_set = true;
									}
									else
										player = gnar->instance(msgc->path);

									m_scene->addChild(player);
									Math::Vector3 position = Math::Vector3(msgc->pos[0], msgc->pos[1], msgc->pos[2]);
									Math::Quaternion rot = Math::Quaternion(msgc->transform[0], msgc->transform[1], msgc->transform[2], msgc->transform[3]);
									player->setPosition(position);
									player->getTransform()->get_position()->setOrientation(rot);
									std::unique_ptr<Rhea::NetworkComponent> net_comp = std::make_unique<Rhea::NetworkComponent>(player, msgc->NUID, NET_ROLE_CLIENT);
									net_comp->setDelegate(this);
									player->setNetworkComponent(std::move(net_comp));
									m_net_items[msgc->NUID] = player;
									Rhea::Event e("scene_enter", player);
									m_scene->bubbleEvent("scene_enter", &e);

									m_scene->addItem(player);
									player->ready();

									if (msgc->is_player) {
										Event ev("world_enter", this);
										sendEvent("world_enter", &ev);
									}
								}
								else {
									LOG(LOG_ERROR) << "Did not find player template";
								}
							}
						}
						else if (msg->base.opcode == HERMES_MSG_POSITION) {
							Rhea::Position pos;
							Hermes::MessagePosition* move_msg = (Hermes::MessagePosition*)msg.get();
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
						else if (msg->base.opcode == HERMES_MSG_DESTROY) {
							// destruction of item requested
							Hermes::MessageDestroy* msgc = (Hermes::MessageDestroy*)msg.get();
							auto it = m_net_items.find(msgc->NUID);
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
						else {
							DLOG(LOG_DEBUG) << "Unexpected message with opcode=" << msg->getOpcode();
						}
					}
					else {
						// TODO, for now assume connection failure
						LOG(LOG_FATAL) << "Server connection lost";
						Event ev("connection_failure", this);
						sendEvent("connection_failure", &ev);
					}
					});
			}
			else {
				// TODO handle login failure
				DLOG(LOG_DEBUG) << "login failed " << err;
				Event ev("login_failure", this);
				sendEvent("login_failure", &ev);
			}
			});
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