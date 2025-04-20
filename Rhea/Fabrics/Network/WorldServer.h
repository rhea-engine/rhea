#ifndef RHEA_NETWORK_WORLD_SERVER_H
#define RHEA_NETWORK_WORLD_SERVER_H

#include <Hermes/Stream.h>
#include <Hermes/Messages/MessageMove.h>
#include <Hermes/Messages/MessageTurn.h>
#include <Rhea/Core/Item.h>

namespace Rhea
{
	class WorldServer
	{
	public:
		WorldServer(Hermes::Stream* stream) : 
			m_stream(stream),
			m_state(CONN_NONE),
			m_player(nullptr)
		{}

		void Initialize();

		void setPlayer(Rhea::Item* player) { m_player = player; }

		void Start();
		void Stop() { m_state = CONN_NONE; }

	protected:
		enum CONN_STATE {
			CONN_NONE,
			CONN_IN_WORLD,
			CONN_CLOSE
		};

		CONN_STATE m_state;

		void onMoveMessage(Hermes::MessageMove* move_msg);
		void onTurnMessage(Hermes::MessageTurn* turn_msg);

		Hermes::Stream* m_stream;
		Rhea::Item* m_player;
	};
}

#endif // RHEA_NETWORK_WORLD_SERVER_H