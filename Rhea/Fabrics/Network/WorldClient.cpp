#include "WorldClient.h"
#include <Hermes/Opcode.h>
#include <Rhea/UnitTree.h>
#include <Rhea/Core/Item.h>
#include <Psio/Log.h>
#include <Hermes/Messages/MessageMove.h>
#include <Hermes/Messages/MessageTurn.h>
#include <Hermes/Messages/MessagePosition.h>
#include <Hermes/Messages/MessageCreate.h>
#include <Hermes/Messages/MessageDestroy.h>
#include <Hermes/Messages/MessageAvatarList.h>
#include <Rhea/Core/Avatar.h>
#include <Hermes/Messages/MessageEnterWorld.h>
#include <Application.h>
#include "ClientService.h"
#include "NetworkComponent.h"

namespace Rhea
{
	void WorldClient::Initialize()
	{
		Hermes::MessageFactory* factory = m_stream->getMessageFactory();
		if (!factory) {
			LOG(LOG_ERROR) << "No message factory found";
			return;
		}
		
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
			Rhea::NetworkComponent* net_comp = new Rhea::NetworkComponent(player, msg->NUID, NET_ROLE_CLIENT);
			net_comp->setDelegate(this);
			player->addComponent(net_comp);
			m_net_items[msg->NUID] = player;
			Rhea::Event e("scene_enter", player);
			m_scene->bubbleEvent("scene_enter", &e);

			m_scene->addItem(player);
			player->ready();

			if (msg->is_player) {
				if (m_on_enter_world)
					m_on_enter_world(0);
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

	void WorldClient::Stop()
	{
		m_in_world = false;
	}
	
	void WorldClient::changeMovement(int nuid, Math::Vector3 move)
	{
		Hermes::MessageMove move_msg;
		move_msg.NUID = nuid;
		move_msg.move[0] = move.x;
		move_msg.move[1] = move.y;
		move_msg.move[2] = move.z;

		m_stream->sendMessage(&move_msg, nullptr);
	}

	void WorldClient::changeTurn(int nuid, float turn)
	{
		Hermes::MessageTurn turn_msg;
		turn_msg.NUID = nuid;
		turn_msg.turn = turn;

		m_stream->sendMessage(&turn_msg, nullptr);
	}
}