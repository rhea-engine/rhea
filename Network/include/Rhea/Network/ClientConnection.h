#ifndef FORTYTWO_CLIENTCONN_H
#define FORTYTWO_CLIENTCONN_H

#include <Psio/Net/Network.h>
#include <Hermes/ServerStream.h>
#include <Rhea/Application.h>
#include <Rhea/Core/Scene.h>
#include <Hermes/Messages/MessageEnterWorld.h>
#include <Hermes/Messages/MessageMove.h>
#include <Hermes/Messages/MessageTurn.h>
#include <Hermes/Messages/MessageCreateAvatar.h>
#include "IConnectionData.h"

namespace rhea
{
	class ClientConnection
	{
	public:
		ClientConnection(rhea::Application* app, rhea::Scene* scene, std::unique_ptr<hermes::Stream> stream) :
			m_app(app),
			m_scene(scene),
			m_player(nullptr),
			m_world(nullptr),
			m_hermes(std::move(stream))
		{}

		int Start(psio::Network::CompletionHandler err_handler);
		int sendMessage(hermes::Message* msg);
		void Close();

		void onClosed(std::function<void(int)> handler) { m_on_closed = handler; }

		void setConnectionData(std::unique_ptr<IConnectionData> data) { m_connection_data = std::move(data); }

		// set the item this connection is responsible for
		void setAuthItem(rhea::Item* item) { m_player = item; }
		// return the item this connection is responsible for
		rhea::Item* getAuthItem() { return m_player; }

		// start with an already connected player
		void StartEstablishedConnection(rhea::Item* player);

		hermes::Stream* getStream() { return m_hermes.get(); }

	protected:

		rhea::Application* m_app;
		rhea::Scene* m_scene;
		rhea::Item* m_player;
		rhea::Unit* m_world;
		std::string m_username;

		std::unique_ptr<hermes::Stream> m_hermes;
		psio::Network::CompletionHandler m_error_handler;

		void CreateUser(std::string method, std::string username, std::string name, std::string ext_id, std::vector<std::string> secrets);
		void Login(std::string method, std::string username, std::vector<std::string> secrets);

		std::function<void(int)> m_on_closed;

		std::unique_ptr<IConnectionData> m_connection_data;
	};
}
#endif // !FORTYTWO_CLIENTCONN_H
