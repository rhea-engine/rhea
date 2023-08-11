#include "ClientConnect.h"
#include <Components/Psio/Log.h>
#include <Rhea/UnitTree.h>
#include <Rhea/Fabrics/GUI/Widget.h>
#include <Rhea/Fabrics/Network/ClientService.h>

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

	void ClientConnect::doConnect()
	{
		if (!m_net)
			return;

		m_connection_state = 1;
		m_values["info"] = "Connecting";

		if (m_info_w)
			m_info_w->show();

		m_net->connect(m_values["server"], [&](Rhea::WorldClient* client, int err) {
			if (!err)
			{
				m_net->addChild(client);
				m_connection_state = 2;
				m_values["info"] = "Sending login information";
				DLOG(LOG_DEBUG) << "connected to server";
				client->setAuth("anonymous");
				Rhea::Scene* avatar_select_scene = static_cast<Rhea::Scene*>(m_app->getUnitTree()->find(getValue("onAvatarSelectScene"), m_attributes["basepath"], false));
				if (avatar_select_scene) {
					client->registerEventHandler("avatarlist", this, [&, avatar_select_scene](Rhea::Event* ev, Rhea::Unit* t)->bool {
						if ((int)t->getValue("nofAvatars", 0) > 0) {
							// goto scene avatar_select_scene
							m_values["info"] = "select avatar";
							if (m_info_w)
								m_info_w->hide();
							if (m_parent && !m_parent->getTypeName().compare("//meworla/scene"))
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
				client->registerEventHandler("login_failure", this, [&](Rhea::Event* ev, Rhea::Unit* t)->bool {
					m_connection_state = 0;
					m_error = 2;
					doError();
					return false;
					});
				client->registerEventHandler("connection_failure", this, [&](Rhea::Event* ev, Rhea::Unit* t)->bool {
					m_connection_state = 0;
					m_error = 1;
					doError();
					return false;
					});

				client->start();
			}
			else {
				m_connection_state = 0;
				m_error = 1;
				doError();
			}
			});
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
		m_net = static_cast<Rhea::ClientService*>(m_app->getUnitTree()->find(m_values["netManager"], m_attributes["basepath"], false));
		m_info_w = static_cast<Rhea::Widget*>(m_app->getUnitTree()->find(m_values["infowidget"], m_attributes["basepath"], false));
		if (m_info_w && !m_info_w->getValue("isWidget"))
			m_info_w = nullptr;
		m_error_w = static_cast<Rhea::Widget*>(m_app->getUnitTree()->find(m_values["errorwidget"], m_attributes["basepath"], false));
		if (m_error_w && !m_error_w->getValue("isWidget"))
			m_error_w = nullptr;

		// register event handlers

		registerEventHandler("scene_show", this, std::bind(&ClientConnect::onDisplay, this, std::placeholders::_1, std::placeholders::_2));
		registerEventHandler("abort", this, std::bind(&ClientConnect::onAbort, this, std::placeholders::_1, std::placeholders::_2));
		registerEventHandler("retry", this, std::bind(&ClientConnect::onRetry, this, std::placeholders::_1, std::placeholders::_2));
	}

	Rhea::Unit* ClientConnect::createType(std::string name, std::string type)
	{
		Rhea::Unit* ret = (Unit*)new ClientConnect(name, type);
		return ret;
	}
}