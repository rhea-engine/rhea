#include "Rhea/Network/ClientConnection.h"
#include <Rhea/UnitTree.h>
#include <Rhea/Core/Item.h>
#include <Rhea/Physics/PhysicsService.h>
#include <Hermes/Messages/MessageCreate.h>
#include <Hermes/Messages/MessageMove.h>
#include <Hermes/Messages/MessageTurn.h>
#include <Hermes/Messages/MessagePosition.h>
#include <Hermes/Messages/MessageAvatarList.h>
#include <Hermes/Messages/MessageEnterWorld.h>
#include <Rhea/Core/Avatar.h>
#include <Psio/Log.h>
#include "Rhea/Network/ServerService.h"
#include <Hermes/Messages/MessageEnterFailed.h>
#include <Hermes/Messages/MessageLogin.h>
#include <Hermes/Messages/MessageCreateAvatar.h>

namespace rhea
{
	int ClientConnection::Start(Psio::Network::CompletionHandler err_handler)
	{
		m_error_handler = err_handler;

		// error handling

		m_hermes->onClosed([&](int err) {
			m_hermes.reset(nullptr);
			if (m_on_closed)
				m_on_closed(err);
			});
		m_hermes->onConnectionError([&](int err) {
			if (m_error_handler)
				m_error_handler(err);
			});

		// start the connection - wait for login	
		return m_hermes->Start();
	}

	int ClientConnection::sendMessage(Hermes::Message* msg)
	{
		// forward to stream
		return m_hermes->sendMessage(msg, [&](int err) {
			if (err < -1) {
				m_hermes->Close();
				if (m_error_handler)
					m_error_handler(err);
			}
			});
	}

	void ClientConnection::Close()
	{
		m_hermes->Close();
	}

	// helper for testing
	void ClientConnection::StartEstablishedConnection(rhea::Item* player)
	{
		m_player = player;
	}
}