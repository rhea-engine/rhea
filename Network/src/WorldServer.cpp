#include "Rhea/Network/WorldServer.h"
#include <Psio/Log.h>

namespace Rhea
{
	void WorldServer::Initialize()
	{
		Hermes::MessageFactory* factory = m_stream->getMessageFactory();
		if (!factory) {
			LOG(LOG_ERROR) << "No message factory found";
			return;
		}

		factory->registerMessageHandler<Hermes::MessageMove>(HERMES_MSG_MOVE,
			[&](uint16_t opcode)->bool {
				return m_state == CONN_IN_WORLD;
			},
			std::bind(&WorldServer::onMoveMessage, this, std::placeholders::_1)
		);

		factory->registerMessageHandler<Hermes::MessageTurn>(HERMES_MSG_TURN,
			[&](uint16_t opcode)->bool {
				return m_state == CONN_IN_WORLD;
			},
			std::bind(&WorldServer::onTurnMessage, this, std::placeholders::_1)
		);

	}

	void WorldServer::Start()
	{
		if (m_player)
		{
			m_state = CONN_IN_WORLD;
		}
		else {
			LOG(LOG_ERROR) << "Player not found";
		}
	}

	void WorldServer::onMoveMessage(Hermes::MessageMove* move_msg)
	{
		m_player->setMovement(Math::Vector3(move_msg->move[0], move_msg->move[1], move_msg->move[2]));
		DLOG(LOG_DEBUG) << "MoveMsg received: " << move_msg->move[0] << ", " << move_msg->move[1] << ", " << move_msg->move[2];
	}

	void WorldServer::onTurnMessage(Hermes::MessageTurn* turn_msg)
	{
		m_player->setTurn(turn_msg->turn);
		DLOG(LOG_DEBUG) << "TurnMsg received: " << turn_msg->turn;
	}


}
