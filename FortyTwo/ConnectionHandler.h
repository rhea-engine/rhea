#ifndef FORTYTWO_CONNECTIONHANDLER_H
#define FORTYTWO_CONNECTIONHANDLER_H

#include <Rhea/Network/IConnectionData.h>
#include <Rhea/Network/ClientConnection.h>
#include "AuthHandler.h"
#include <Rhea/Network/LobbyServer.h>
#include <Rhea/Network/WorldServer.h>

class ConnectionHandler : public rhea::IConnectionData
{
public:
	ConnectionHandler(rhea::Application* app, rhea::Scene* scene) :
		m_app(app),
		m_user(nullptr),
		m_scene(scene),
		m_player(nullptr),
		m_connection(nullptr)
	{}
	void Initialize(Rhea::ClientConnection* connection);
protected:
	std::unique_ptr<hermes::ServerStream> server_stream;
	std::unique_ptr<AuthHandler> auth_handlers;
	std::unique_ptr<rhea::LobbyServer> lobby_server;
	std::unique_ptr<rhea::WorldServer> world_server;
	User* m_user;
	rhea::Application* m_app;
	rhea::Item* m_player;
	rhea::Scene* m_scene;
	Rhea::ClientConnection* m_connection;

	int EnterWorld(rhea::Avatar* avatar);
};
#endif //FORTYTWO_CONNECTIONHANDLER_H