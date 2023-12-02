#include "ClientConnect.h"
#include <Components/Psio/Log.h>
#include <Rhea/UnitTree.h>
#include <Rhea/Fabrics/GUI/Widget.h>
#include <Rhea/Fabrics/Network/ClientService.h>
#include <CredentialsService.h>

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

	bool ClientConnect::onLoginTrust(Rhea::Event* ev, Rhea::Unit* unit)
	{
		if (m_username != "") {
			m_trust_w->hide();
			m_info_w->show();
			m_client->setAuth("trust", m_username, std::vector<std::string>());
			m_client->start();
			return false;
		}
	}

	bool ClientConnect::onSetUsername(Rhea::Event* ev, Rhea::Unit* unit)
	{
		m_username = ev->getTarget()->getValue("text", "");
		return false;
	}

	void ClientConnect::doConnect()
	{
		if (!m_net) {
			m_values["info"] = "No network defined!";
			if (m_info_w)
				m_info_w->show();
			return;
		}

		m_connection_state = 1;
		m_values["info"] = "Connecting";

		if (m_info_w)
			m_info_w->show();

		m_net->Connect(m_values["server"], [&](Rhea::WorldClient* client, int err) {
			if (!err)
			{
				m_client.reset(client);
				m_net->addChild(client);
				m_connection_state = 2;
				m_values["info"] = "Sending login information";
				DLOG(LOG_DEBUG) << "connected to server";				
				Rhea::Scene* avatar_select_scene = doCast<Rhea::Scene>(m_app->getUnitTree()->find(getValue("onAvatarSelectScene"), m_attributes["basepath"], false));
				if (avatar_select_scene) {
					client->registerEventHandler("avatarlist", this, [&, avatar_select_scene](Rhea::Event* ev, Rhea::Unit* t)->bool {
						if ((int)t->getValue("nofAvatars", 0) > 0) {
							// goto scene avatar_select_scene
							m_values["info"] = "select avatar";
							if (m_info_w)
								m_info_w->hide();
							if (doCast<Rhea::Scene>(m_parent))
								static_cast<Rhea::Scene*>(m_parent)->hide();
							avatar_select_scene->show();
						}
						else {
							// goto new avatar scene
							m_values["info"] = "no avatar defined";
						}
						return false;
						});
				}
				else {
					if (m_values.find("onAvatarSelectScene") == m_values.end())
						m_values["info"] = "Configuration error: \nNo onAvatarSelectScene defined in ClientConnect";
					else
						m_values["info"] = std::string("Configuration error: \nonAvatarSelectScene '" + (std::string)getValue("onAvatarSelectScene") + "' not found in ClientConnect");
				}
				client->registerEventHandler("login_failure", this, [&](Rhea::Event* ev, Rhea::Unit* t)->bool {

					// do we have more methods?
					// if we get here we have no stored credentials that match -> offer all methods for new login
					OfferAuthMethods();
					return false;
					});
				client->registerEventHandler("connection_failure", this, [&](Rhea::Event* ev, Rhea::Unit* t)->bool {
					m_connection_state = 0;
					m_error = 1;
					doError();
					return false;
					});

				

				doLogin();

			}
			else {
				m_connection_state = 0;
				m_error = 1;
				doError();
			}
			});
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
		std::string world = m_app->getUnitTree()->getChild("engine")->getValue("origin");
		std::vector<std::string> methods;
		methods.push_back("password");
		methods.push_back("trust");
		methods.push_back("anonymous");
		CredentialsService::Credential* credential = m_cred_srv->getCredential(world, methods);
		if (credential) {
			m_client->setAuth(credential->method, credential->username, credential->secrets);
			delete credential;
			m_client->start();
		}
		else {
			// try to use anonymous
			m_client->setAuth("trust", "anonymous", std::vector<std::string>());
			m_client->start();
		}
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

	void ClientConnect::doError()
	{
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
		m_net = isCast<Rhea::ClientService>(m_app->getUnitTree()->find(m_values["netManager"], m_attributes["basepath"], false));
		m_info_w = doCast<Rhea::Widget>(m_app->getUnitTree()->find(m_values["infowidget"], m_attributes["basepath"], false));
		m_error_w = doCast<Rhea::Widget>(m_app->getUnitTree()->find(m_values["errorwidget"], m_attributes["basepath"], false));
		m_trust_w = doCast<Rhea::Widget>(m_app->getUnitTree()->find(m_values["logintrustwidget"], m_attributes["basepath"], false));

		// register event handlers

		registerEventHandler("scene_show", this, std::bind(&ClientConnect::onDisplay, this, std::placeholders::_1, std::placeholders::_2));
		registerEventHandler("abort", this, std::bind(&ClientConnect::onAbort, this, std::placeholders::_1, std::placeholders::_2));
		registerEventHandler("retry", this, std::bind(&ClientConnect::onRetry, this, std::placeholders::_1, std::placeholders::_2));
		registerEventHandler("login", this, std::bind(&ClientConnect::onLoginTrust, this, std::placeholders::_1, std::placeholders::_2));
		registerEventHandler("setUsername", this, std::bind(&ClientConnect::onSetUsername, this, std::placeholders::_1, std::placeholders::_2));
	}
}