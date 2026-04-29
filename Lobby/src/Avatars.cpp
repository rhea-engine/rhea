/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#include <Rhea/Lobby/Avatars.h>

#include <Rhea/Application.h>
#include <Rhea/UnitTree.h>
#include <Rhea/GUI/Widget.h>
#include <Rhea/GUI/List.h>
#include <Rhea/Core/Avatar.h>
#include <Psio/Log.h>

namespace Meworla
{
	bool Avatars::onDisplay(rhea::Event* ev, rhea::Unit* unit)
	{
		// if network is not defined this is an configuration error
		if (!m_net) {
			LOG(LOG_ERROR) << "No network defined.";
			return false;
		}

		static bool initialized = false;
		if (!initialized) {			
			m_connector = isCast<Meworla::ClientConnect>(m_app->getUnitTree()->find(m_values["connector"],m_attributes["basepath"], false));
			if (!m_connector) {
				LOG(LOG_ERROR) << "No connector defined.";
				return false;
			}

			m_connector->registerEventHandler("world_enter", this, std::bind(&Avatars::onWorldEnter, this, std::placeholders::_1, std::placeholders::_2));
			m_connector->registerEventHandler("create_avatar_failed", this, std::bind(&Avatars::onCreateFailed, this, std::placeholders::_1, std::placeholders::_2));

			// avatar select			
			registerEventHandler("open", this, std::bind(&Avatars::onOpen, this, std::placeholders::_1, std::placeholders::_2));
			registerEventHandler("callCreate", this, std::bind(&Avatars::onOpen, this, std::placeholders::_1, std::placeholders::_2));

			// new avatar widget
			registerEventHandler("setAvatarname", this, std::bind(&Avatars::onSetAvatarname, this, std::placeholders::_1, std::placeholders::_2));
			registerEventHandler("setAvatarType", this, std::bind(&Avatars::onSetAvatarType, this, std::placeholders::_1, std::placeholders::_2));
			registerEventHandler("create_avatar", this, std::bind(&Avatars::onCreate, this, std::placeholders::_1, std::placeholders::_2));
			registerEventHandler("cancel_create_avatar", this, std::bind(&Avatars::onCreateCancel, this, std::placeholders::_1, std::placeholders::_2));
			m_avatar_w->registerEventHandler("new_avatar", this, std::bind(&Avatars::onCallCreate, this, std::placeholders::_1, std::placeholders::_2));

			initialized = true;
		}

		// fill list?
		rhea::Unit* avatar_list = doCast<rhea::Unit>(m_connector->find("avatarlist"));
		std::vector<Variant> avatars_arr;		
		if (avatar_list) {
			avatar_list->children([&avatars_arr](std::string& name, rhea::Component* unit)->bool {
				rhea::Avatar* avatar = isCast<rhea::Avatar>(unit);
				if (avatar) {
					avatars_arr.push_back(name);
				}
				return true;
				});
		}
		else {
			LOG(LOG_ERROR) << "No avatar list defined.";			
		}
		m_values["avatars"] = avatars_arr;		

		// do we have avatars?
		if (!avatars_arr.size() && m_newavatar_w) {
			// no -> show create avatar window
			m_newavatar_w->show();
			return false;
		}

		// show the selection widget
		if (m_avatar_w) {
			m_avatar_w->show();
		}
		else {
			LOG(LOG_ERROR) << "No avatar widget defined.";
		}

		return false;
	}

	void Avatars::ready()
	{
		m_avatar_w = doCast<rhea::Widget>(m_app->getUnitTree()->find(m_values["avatarwidget"], m_attributes["basepath"], false));
		m_newavatar_w = doCast<rhea::Widget>(m_app->getUnitTree()->find(m_values["newavatarwidget"], m_attributes["basepath"], false));

		std::vector<Variant> avatars_arr;
		m_values["avatars"] = avatars_arr;

		m_net = doCast<Rhea::ClientService>(m_app->getUnitTree()->find(m_values["netManager"], m_attributes["basepath"], false));
		if (!m_net) {
			LOG(LOG_ERROR) << "No network defined.";
			return;
		}

		registerEventHandler("scene_show", this, std::bind(&Avatars::onDisplay, this, std::placeholders::_1, std::placeholders::_2));
	}

	bool Avatars::onSetAvatarname(rhea::Event* ev, rhea::Unit* unit)
	{
		m_avatar_name = ev->getTarget()->getValue("text", "");
		return false;
	}

	bool Avatars::onSetAvatarType(rhea::Event* ev, rhea::Unit* unit)
	{
		m_avatar_type = ev->getTarget()->getValue("text", "");
		return false;
	}

	bool Avatars::onCreate(rhea::Event* ev, rhea::Unit* unit)
	{
		m_avatar_type = "//local/scene2/gus";
		if (m_avatar_name.length() && m_avatar_type.length()) {
			m_newavatar_w->hide();

			// scene already loaded?
			rhea::Scene* entry_scene = doCast<rhea::Scene>(m_app->getUnitTree()->find(m_values["entrypoint"]));
			if (entry_scene) {
				m_connector->setScene(entry_scene);
				m_connector->CreateAvatar(m_avatar_name, m_avatar_type);
			}
			else {
				// scene must be loaded first

				m_app->loadResource(m_values["entryscene"], "//meworla/scene", [&](rhea::Unit* u, ReqError err) {
					if (err == ReqError::OK) {
						rhea::Scene* entry_scene = doCast<rhea::Scene>(m_app->getUnitTree()->find(m_values["entrypoint"]));
						if (!entry_scene) {
							LOG(LOG_ERROR) << "No entrypoint defined.";
							return;
						}
						m_connector->setScene(entry_scene);
						m_connector->CreateAvatar(m_avatar_name, m_avatar_type);
					}
					});
			}			
		}
		return false;
	}

	bool Avatars::onCreateCancel(rhea::Event* ev, rhea::Unit* unit)
	{
		m_newavatar_w->hide();
		m_avatar_w->show();
		return false;
	}

	bool Avatars::onCreateFailed(rhea::Event* ev, rhea::Unit* unit)
	{
		m_newavatar_w->show();
		return false;
	}

	bool Avatars::onOpen(rhea::Event* ev, rhea::Unit* unit)
	{	
		if (m_avatar_w) {
			m_avatar_w->hide();
		}		
		ev->stopPropagation();

		Unit* target = ev->getTarget();
		DLOG(LOG_DEBUG) << "target selected was '" << (std::string)target->getValue("value") << "'";
		std::string avatar_name = (std::string)target->getValue("value");

		m_app->loadResource(m_values["entryscene"], "//meworla/scene", [&, avatar_name](rhea::Unit* u, ReqError err) {
			if (err == ReqError::OK) {
				rhea::Scene* entry_scene = doCast<rhea::Scene>(m_app->getUnitTree()->find(m_values["entrypoint"]));
				if (!entry_scene) {
					LOG(LOG_ERROR) << "No entrypoint defined.";
					return;
				}
				m_connector->setScene(entry_scene);
				m_connector->EnterWorld(avatar_name);
			}
			});

		return false;
	}

	bool Avatars::onCallCreate(rhea::Event* ev, rhea::Unit* unit)
	{
		if (m_newavatar_w) {
			m_avatar_w->hide();
			m_newavatar_w->show();
		}
		return false;
	}

	bool Avatars::onWorldEnter(rhea::Event* ev, rhea::Unit* unit)
	{
		rhea::Scene* entry_scene = doCast<rhea::Scene>(m_app->getUnitTree()->find(getValue("entrypoint"), m_attributes["basepath"], false));
		if (entry_scene) {
			LOG(LOG_INFO) << "Entering world scene";
			if (doCast<rhea::Scene>(m_parent))
				static_cast<rhea::Scene*>(m_parent)->hide();
			entry_scene->show();
		}
		else {
			// need to load the scene
			m_app->loadResource(m_values["entryscene"], "//meworla/scene", [&](rhea::Unit* u, ReqError err) {
				if (err == ReqError::OK) {
					rhea::Scene* entry_scene = doCast<rhea::Scene>(m_app->getUnitTree()->find(m_values["entrypoint"]));
					if (entry_scene) {
						if (doCast<rhea::Scene>(m_parent))
							static_cast<rhea::Scene*>(m_parent)->hide();
						entry_scene->show();
					}
					else {
						if (m_values.find("entrypoint") == m_values.end())
							LOG(LOG_ERROR) << "The entrypoint was not defined in the avatar configuration.";
						else
							LOG(LOG_ERROR) << "The entrypoint was not found in the scene.";
					}
				}
				else {
					if (m_values.find("entryscene") == m_values.end())
						LOG(LOG_ERROR) << "The entryscene was not defined in the avatar configuration.";
					else
						LOG(LOG_ERROR) << "Avatars::onWorldEnter: Could not open scene.";
				}
				});
		}
		return false;
	}
}
