#ifndef FORTYTWO_CLIENTCONN_H
#define FORTYTWO_CLIENTCONN_H

#include <Psio/Net/Network.h>
#include <Hermes/ServerStream.h>
#include <Rhea/Application.h>
#include <Rhea/Fabrics/Core/Scene.h>
#include <FortyTwo/Accounts/User.h>
#include "World.h"
#include <Hermes/Messages/MessageEnterWorld.h>
#include <Hermes/Messages/MessageMove.h>
#include <Hermes/Messages/MessageTurn.h>
#include <Hermes/Messages/MessageCreateAvatar.h>

class ClientConnection
{
public:
	ClientConnection(Rhea::Application* app, Rhea::Scene* scene, std::unique_ptr<Hermes::ServerStream> hermes) :
		m_app(app),
		m_scene(scene),
		m_player(nullptr),
		m_world(nullptr),
		m_state(CONN_NONE),
		m_user(nullptr)
	{
		m_hermes = std::move(hermes);
	}

	int Start(Psio::Network::CompletionHandler err_handler);
	int sendMessage(Hermes::Message* msg);
	void Close();

	// return the item this connection is responsible for
	Rhea::Item* getAuthItem() { return m_player; }

	// start with a connected user
	void StartConnectedUser(User* user);
	// start with an already connected player
	void StartEstablishedConnection(Rhea::Item* player);

	int EnterWorld(Rhea::Avatar* avatar);

protected:
	
	Rhea::Application* m_app;
	Rhea::Scene* m_scene;
	Rhea::Item* m_player;
	Rhea::Unit* m_world;
	std::string m_username;
	User* m_user;

	std::unique_ptr<Hermes::ServerStream> m_hermes;
	Psio::Network::CompletionHandler m_error_handler;

	enum CONN_STATE {
		CONN_NONE,
		CONN_LOGIN,		
		CONN_LOBBY,
		CONN_IN_WORLD,
		CONN_CLOSE
	};

	CONN_STATE m_state;

	void onGetAvatarListMessage(Hermes::Message* msg);
	void onCreateAvatarMessage(Hermes::MessageCreateAvatar* msg);
	void onEnterWorldMessage(Hermes::MessageEnterWorld* msg);
	void onMoveMessage(Hermes::MessageMove* msg);
	void onTurnMessage(Hermes::MessageTurn* msg);

	void CreateUser(std::string method, std::string username, std::string name, std::string ext_id, std::vector<std::string> secrets);
	void Login(std::string method, std::string username, std::vector<std::string> secrets);
};

#endif // !FORTYTWO_CLIENTCONN_H
