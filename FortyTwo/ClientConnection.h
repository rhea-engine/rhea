#ifndef FORTYTWO_CLIENTCONN_H
#define FORTYTWO_CLIENTCONN_H

#include <Psio/Net/Network.h>
#include <Hermes/ServerStream.h>
#include <Rhea/Application.h>
#include <Rhea/Fabrics/Core/Scene.h>
#include <FortyTwo/Accounts/User.h>
#include "World.h"

class ClientConnection
{
public:
	ClientConnection(Rhea::Application* app, Rhea::Scene* scene, std::unique_ptr<Hermes::ServerStream> hermes) :
		m_app(app),
		m_scene(scene),
		m_player(nullptr),
		m_world(nullptr),
		m_state(CONN_NONE),
		m_next_state(CONN_NONE),
		m_user(nullptr)
	{
		m_hermes = std::move(hermes);
	}

	int Start(Psio::Network::CompletionHandler err_handler);
	int sendMessage(Hermes::Message* msg);

	// return the item this connection is responsible for
	Rhea::Item* getAuthItem() { return m_player; }

	// start with a connected user
	void StartConnectedUser(User* user);
	// start with an already connected player
	void StartEstablishedConnection(Rhea::Item* player);

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
		CONN_LOGIN_COMPLETED,
		CONN_LOBBY,
		CONN_AVATAR,
		CONN_AVATAR_COMPLETED,
		CONN_MESSAGE,
		CONN_MESSAGE_COMPLETED,
		CONN_CLOSE,
		CONN_CLOSE_COMPLETED
	};

	CONN_STATE m_state;
	CONN_STATE m_next_state;

	int DoLoop(int result);
	void CallDoLoop(int result);
	int DoLogin(int result);
	void Login(std::string method, std::string username, std::vector<std::string> secrets);
	int DoLoginCompleted(int result);
	int DoLobby(int result);
	int DoAvatar(int result);
	int DoAvatarCompleted(int result);
	int DoMessage(int result);
	int DoMessageCompleted(int result);
	int DoClose(int result);
	int DoCloseCompleted(int result);

	std::unique_ptr<Hermes::Message> m_cur_msg;
};

#endif // !FORTYTWO_CLIENTCONN_H
