#ifndef FORTYTWO_CONNECTIONHANDLER_H
#define FORTYTWO_CONNECTIONHANDLER_H

#include <Rhea/Network/IConnectionData.h>
#include <Rhea/Network/ClientConnection.h>
#include "AuthHandler.h"
#include <Rhea/Network/LobbyServer.h>
#include <Rhea/Network/WorldServer.h>

class ConnectionHandler : public Rhea::IConnectionData
{
public:
	ConnectionHandler(Rhea::Application* app, Rhea::Scene* scene) :
		m_app(app),
		m_user(nullptr),
		m_scene(scene),
		m_player(nullptr),
		m_connection(nullptr)
	{}
	void Initialize(Rhea::ClientConnection* connection);
protected:
	std::unique_ptr<Hermes::ServerStream> server_stream;
	std::unique_ptr<AuthHandler> auth_handlers;
	std::unique_ptr<Rhea::LobbyServer> lobby_server;
	std::unique_ptr<Rhea::WorldServer> world_server;
	User* m_user;
	Rhea::Application* m_app;
	Rhea::Item* m_player;
	Rhea::Scene* m_scene;
	Rhea::ClientConnection* m_connection;

	int EnterWorld(Rhea::Avatar* avatar);
};
#endif //FORTYTWO_CONNECTIONHANDLER_H