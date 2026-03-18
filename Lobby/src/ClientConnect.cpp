#include <Rhea/Lobby/ClientConnect.h>
#include <Components/Psio/Log.h>
#include <Rhea/UnitTree.h>
#include <Rhea/GUI/Widget.h>
#include <Rhea/Fabrics/Network/ClientService.h>
#include <Rhea/Lobby/CredentialsService.h>

namespace Meworla
{
	bool ClientConnect::onDisplay(Rhea::Event* ev, Rhea::Unit* unit)
	{
		doConnect();
		return false;
	}

	bool ClientConnect::onAbort(Rhea::Event* ev, Rhea::Unit* unit)
	{
		// quit the application?
		exit(0);
		return false;
	}

	bool ClientConnect::onRetry(Rhea::Event* ev, Rhea::Unit* unit)
	{
		if (m_error_w)
			m_error_w->hide();
		if (!(m_connection_state == 1))
			doConnect();
		else
			LOG(LOG_ERROR) << "ClientConnect.ready() called while connecting. Ignoring.";
		return false;
	}

	void ClientConnect::CreateAvatar(std::string name, std::string type_name)
	{
		m_lobby_client->Stop();
		m_world_client->Start();
		m_lobby_client->CreateAvatar(name, type_name);
	}

	void ClientConnect::EnterWorld(std::string avatar_name)
	{
		m_lobby_client->Stop();
		m_world_client->Start();
		m_lobby_client->EnterWorld(avatar_name);
	}

	bool ClientConnect::onLoginTrust(Rhea::Event* ev, Rhea::Unit* unit)
	{
		if (m_username != "") {
			m_trust_w->hide();
			m_info_w->show();
			m_auth_client->Login("trust", m_username, std::vector<std::string>(), [&](int err) {
				if (!err) {
					// login success
					onLogin();
				}
				else {
					// login failed - show login window
					OfferAuthMethods();
				}
				});
		}
		return false;
	}

	bool ClientConnect::onRegister(Rhea::Event* ev, Rhea::Unit* unit)
	{
		if (m_username != "") {
			m_register_w->hide();
			m_info_w->show();
			if (m_name == "")
				m_name = m_username;
			m_auth_client->Register("trust", m_username, m_name, "", std::vector<std::string>(), [&](int err) {
				if (!err) {
					// register success
					onLogin();
				}
				else {
					m_values["info"] = "Registering user failed";
					// register failed - show register window
					m_info_w->hide();
					m_trust_w->hide();
					m_register_w->show();
				}
				});
		}
		return false;
	}

	bool ClientConnect::onRegisterFailed(Rhea::Event* ev, Rhea::Unit* unit)
	{
		m_values["info"] = "Registering user failed";
		return false;
	}

	bool ClientConnect::onCallRegister(Rhea::Event* ev, Rhea::Unit* unit)
	{
		if (m_register_w) {
			m_info_w->hide();
			m_trust_w->hide();
			m_register_w->show();
		}
		else {
			m_values["info"] = "Configuration error: \nNo register widget defined in ClientConnect!";			
		}
		return false;
	}

	bool ClientConnect::onSetUsername(Rhea::Event* ev, Rhea::Unit* unit)
	{
		m_username = ev->getTarget()->getValue("text", "");
		return false;
	}

	void ClientConnect::doConnect()
	{
		m_connection_state = 1;
		m_values["info"] = "Connecting";

		if (m_info_w)
			m_info_w->show();

		Rhea::ClientService* service = doCast<Rhea::ClientService>(m_app->getService("hermesclient"));
		if (!service) {
			LOG(LOG_ERROR) << "Configuration error: no hermesclient service defined";
			m_error = 1;
			m_values["error"] = "Configuration error: \nNo hermesclient service defined in ClientConnect!";
			Rhea::Event ev("error", this);
			sendEvent("error", &ev);
			return;
		}

		service->Connect(m_values["server"], [&, service](Hermes::Stream* stream, int err) {
			if (!err) {
				m_stream.reset(stream);

				m_connection_state = 2;
				m_values["info"] = "Sending login information";
				DLOG(LOG_DEBUG) << "connected to server";
				onConnected(); // calls start
				return;
			}
			else {
				m_error = 1;
				doError();
				
				Rhea::Event ev("error", this);
				sendEvent("error", &ev);
			}
			});
	}

	void ClientConnect::onConnected()
	{
		// setup the protocols

		m_auth_client = std::make_unique<Hermes::ClientStream>(m_stream.get());
		m_auth_client->Initialize();

		m_lobby_client = std::make_unique<Rhea::LobbyClient>(m_app, m_stream.get());
		m_lobby_client->Initialize();

		m_world_client = std::make_unique<Rhea::WorldClient>(m_app, m_stream.get());
		m_world_client->Initialize();

		m_world_client->onWorldEnter([&](int err) {
			m_in_world = true;
			Rhea::Event ev("world_enter", this);
			sendEvent("world_enter", &ev);
			});

		// error handling
		m_stream->onConnectionError([&](int err) {
			LOG(LOG_ERROR) << "Connection error: " << err;
			m_error = 1;
			doError();
			});

		// start the client
		doLogin();
	}

	void ClientConnect::doLogin()
	{
		if (!m_cred_srv) {
			m_cred_srv = doCast<CredentialsService>(findByType("//meworla/lobby/credentials"));
			if (!m_cred_srv) {
				m_cred_srv = new CredentialsService("credentials", "//meworla/lobby/credentials");
				m_cred_srv->setup(m_app);
				addChild(m_cred_srv);
			}
		}

		// get credentials
		Rhea::Unit* engine = doCast<Rhea::Unit>(m_app->getUnitTree()->getChild("engine"));
		std::string world = engine->getValue("origin");
		std::vector<std::string> methods;
		methods.push_back("password");
		methods.push_back("trust");
		methods.push_back("anonymous");
		CredentialsService::Credential* credential = m_cred_srv->getCredential(world, methods);
		std::string method;
		std::string username;
		std::vector<std::string> secrets;
		if (credential) {
			method = credential->method;
			username = credential->username;
			secrets = credential->secrets;
			delete credential;
		}
		else {
			// try to use anonymous
			method = "trust";
			username = "anonymous";
			secrets = std::vector<std::string>();
		}

		m_auth_client->Login(method, username, secrets, [&](int err) {
			if (!err) {
				// login success
				onLogin();
			}
			else {
				// login failed
				// do we have more methods?
				// if we get here we have no stored credentials that match -> offer all methods for new login
				OfferAuthMethods();
			}
			});
	}

	void ClientConnect::OfferAuthMethods()
	{
		m_info_w->hide();
		if (!m_trust_w) {
			LOG(LOG_ERROR) << "no trust widget defined";
			return;
		}
		m_username = "";
		m_trust_w->show();
	}

	void ClientConnect::onLogin()
	{
		// enter lobby

		m_auth_client->Stop();

		m_lobby_client->setAvatarOwner(this);

		// handle lobby events
		Rhea::Scene* avatar_select_scene = doCast<Rhea::Scene>(m_app->getUnitTree()->find(getValue("onAvatarSelectScene"), m_attributes["basepath"], false));
		if (avatar_select_scene) {
			m_lobby_client->onAvatarlist([&, avatar_select_scene](int err) {
				// goto scene avatar_select_scene
				m_values["info"] = "select avatar";
				if (m_info_w)
					m_info_w->hide();
				if (doCast<Rhea::Scene>(m_parent))
					static_cast<Rhea::Scene*>(m_parent)->hide();
				avatar_select_scene->show();
				});
		}
		else {
			if (m_values.find("onAvatarSelectScene") == m_values.end())
				m_values["info"] = "Configuration error: \nNo onAvatarSelectScene defined in ClientConnect";
			else
				m_values["info"] = std::string("Configuration error: \nonAvatarSelectScene '" + (std::string)getValue("onAvatarSelectScene") + "' not found in ClientConnect");
		}

		m_lobby_client->onCreateFailed([&](int err) {
			// inform user
			Rhea::Event ev("create_avatar_failed", this);
			sendEvent("create_avatar_failed", &ev);
			});
		m_lobby_client->onEnterFailed([&](int err) {
			Rhea::Event ev("entering_failed", this);
			sendEvent("entering_failed", &ev);
			});

		// start lobby		
		m_lobby_client->Start();
	}

	void ClientConnect::doError()
	{
		if (m_in_world) {
			// we are in the world 
			// we must open the connect scene
			m_connection_state = 1;
			m_values["info"] = "Connecting";
			m_error = 1;

			if (doCast<Rhea::Scene>(m_parent))
				static_cast<Rhea::Scene*>(m_parent)->show();
		}

		if (m_info_w)
			m_info_w->hide();
		m_values["info"] = "No Connection.";

		switch (m_error) {
		case 1:
			m_values["error"] = "Connection failed.";
			break;
		case 2:
			m_values["error"] = "Login failed";
			break;
		}
		if (m_error_w)
			m_error_w->show();
	}

	void ClientConnect::ready()
	{
		// preapre my references
		m_net = doCast<Rhea::ClientService>(m_app->getService("hermesclient"));
		m_info_w = doCast<Rhea::Widget>(m_app->getUnitTree()->find(m_values["infowidget"], m_attributes["basepath"], false));
		m_error_w = doCast<Rhea::Widget>(m_app->getUnitTree()->find(m_values["errorwidget"], m_attributes["basepath"], false));
		m_trust_w = doCast<Rhea::Widget>(m_app->getUnitTree()->find(m_values["logintrustwidget"], m_attributes["basepath"], false));
		m_register_w = doCast<Rhea::Widget>(m_app->getUnitTree()->find(m_values["registerwidget"], m_attributes["basepath"], false));
		
		// register event handlers

		registerEventHandler("scene_show", this, std::bind(&ClientConnect::onDisplay, this, std::placeholders::_1, std::placeholders::_2));
		registerEventHandler("abort", this, std::bind(&ClientConnect::onAbort, this, std::placeholders::_1, std::placeholders::_2));
		registerEventHandler("retry", this, std::bind(&ClientConnect::onRetry, this, std::placeholders::_1, std::placeholders::_2));
		registerEventHandler("login", this, std::bind(&ClientConnect::onLoginTrust, this, std::placeholders::_1, std::placeholders::_2));
		if (m_register_w) {
			registerEventHandler("show_register", this, std::bind(&ClientConnect::onCallRegister, this, std::placeholders::_1, std::placeholders::_2));
			registerEventHandler("register", this, std::bind(&ClientConnect::onRegister, this, std::placeholders::_1, std::placeholders::_2));
		}
		registerEventHandler("setUsername", this, std::bind(&ClientConnect::onSetUsername, this, std::placeholders::_1, std::placeholders::_2));
		
	}
}