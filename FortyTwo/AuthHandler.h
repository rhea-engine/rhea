#ifndef FORTYTWO_AUTHHANDLER_H
#define FORTYTWO_AUTHHANDLER_H

#include <Psio/Net/Network.h>
#include <Hermes/ServerStream.h>
#include <Rhea/Application.h>
#include <Rhea/Core/Scene.h>
#include <FortyTwo/Accounts/User.h>
#include <Hermes/Messages/MessageEnterWorld.h>
#include <Hermes/Messages/MessageMove.h>
#include <Hermes/Messages/MessageTurn.h>
#include <Hermes/Messages/MessageCreateAvatar.h>

// class for holding the connection information and the stream

class AuthHandler 
{
public:
	AuthHandler(rhea::Application* app, rhea::Scene* scene, hermes::ServerStream* hermes) :
		m_app(app),
		m_scene(scene),
		m_player(nullptr),
		m_world(nullptr),
		m_state(CONN_NONE),
		m_user(nullptr)
	{
		m_hermes = hermes;
	}

	void Initialize();

	void onAuthSuccess(std::function<void(User* user)> on_succes_handler) { m_on_succes_handler = on_succes_handler; }

	// return the item this connection is responsible for
	rhea::Item* getAuthItem() { return m_player; }


protected:

	rhea::Application* m_app;
	rhea::Scene* m_scene;
	rhea::Item* m_player;
	rhea::Unit* m_world;
	std::string m_username;
	User* m_user;

	hermes::ServerStream* m_hermes;
	psio::Network::CompletionHandler m_error_handler;
	std::function<void(User* user)> m_on_succes_handler;

	enum CONN_STATE {
		CONN_NONE,
		CONN_LOGIN,
		CONN_CLOSE
	};

	CONN_STATE m_state;

	void CreateUser(std::string method, std::string username, std::string name, std::string ext_id, std::vector<std::string> secrets);
	void Login(std::string method, std::string username, std::vector<std::string> secrets);
};

#endif // !FORTYTWO_AUTHHANDLER_H
