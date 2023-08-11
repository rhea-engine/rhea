#include "Avatars.h"

#include <Rhea/Application.h>
#include <Rhea/UnitTree.h>
#include <Rhea/Fabrics/GUI/Widget.h>
#include <Rhea/Fabrics/GUI/List.h>
#include <Rhea/Fabrics/Core/Avatar.h>
#include <Psio/Log.h>

namespace Meworla
{
	bool Avatars::onDisplay(Rhea::Event* ev, Rhea::Unit* unit)
	{
		// if a dialog is set - show it
		Unit* dlg = m_app->getUnitTree()->find(m_values["avatarwidget"], m_attributes["basepath"], false);
		if (dlg) {
			if (dlg->getValue("isWidget", false)) {
				Rhea::Widget* w = static_cast<Rhea::Widget*>(dlg);
				w->show();
			}
		}

		// fill list?
		Unit* list = m_app->getUnitTree()->find(m_values["avatarlist"], m_attributes["basepath"], false);
		if (list) {
			Rhea::Unit* avatar_list = m_net->findByType("//meworla/avatarlist");
			if (avatar_list) {
				avatar_list->children([&list](std::string& name, Rhea::Unit* unit)->bool {
					if (unit->getTypeName() == "//meworla/avatar") {
						Rhea::Avatar* avatar = (Rhea::Avatar*)unit;
						static_cast<Rhea::ListWidget*>(list)->addItem(name, name);
					}
					return true;
					});
			}
		}
		return false;
	}

	void Avatars::ready()
	{
		m_net = static_cast<Rhea::ClientService*>(m_app->getUnitTree()->find(m_values["netManager"], m_attributes["basepath"], false));
		registerEventHandler("scene_show", this, std::bind(&Avatars::onDisplay, this, std::placeholders::_1, std::placeholders::_2));
		registerEventHandler("open", this, std::bind(&Avatars::onOpen, this, std::placeholders::_1, std::placeholders::_2));
	}

	bool Avatars::onOpen(Rhea::Event* ev, Rhea::Unit* unit)
	{
		Rhea::Widget* w = static_cast<Rhea::Widget*>(m_app->getUnitTree()->find(m_values["avatarwidget"], m_attributes["basepath"], false));
		if (w) {
			w->hide();
		}
		ev->stopPropagation();

		Unit* target = ev->getTarget();
		DLOG(LOG_DEBUG) << "target selected was '" << (std::string)target->getValue("value") << "'";
		std::string avatar_name = (std::string)target->getValue("value");

		// enter the world with given avatarname in the entryscene
		if (avatar_name.length()) {
			Rhea::Scene* entryscene = static_cast<Rhea::Scene*>(m_app->getUnitTree()->find(m_values["entrypoint"]));
			if (entryscene && !entryscene->getTypeName().compare("//meworla/scene")) {
				enterWithAvatar(entryscene, avatar_name);
			}
			else {
				// need to load the scene
				m_app->loadResource(m_values["entryscene"], "//meworla/scene", [&, avatar_name](Rhea::Unit* u, ReqError err) {
					if (err == ReqError::OK) {
						Rhea::Scene* entryscene = static_cast<Rhea::Scene*>(m_app->getUnitTree()->find(m_values["entrypoint"]));
						if (entryscene && !entryscene->getTypeName().compare("//meworla/scene")) {
							enterWithAvatar(entryscene, avatar_name);
						}
						else {
							LOG(LOG_ERROR) << "The entrypoint was not found in the scene.";
						}
					}
					else {
						LOG(LOG_ERROR) << "Could not open scene.";
					}
					});
			}
		}

		return false;
	}

	void Avatars::enterWithAvatar(Rhea::Scene* world_scene, std::string avatar_name)
	{
		Rhea::WorldClient* client = static_cast<Rhea::WorldClient*>(m_net->getChild("worldclient"));
		if (client) {
			if (world_scene) {
				client->setScene(world_scene);
			}
			client->registerEventHandler("world_enter", this, std::bind(&Avatars::onWorldEnter, this, std::placeholders::_1, std::placeholders::_2));

			client->enterWorld(avatar_name);
		}
	}

	bool Avatars::onWorldEnter(Rhea::Event* ev, Rhea::Unit* unit)
	{
		Rhea::Scene* world_scene = static_cast<Rhea::Scene*>(m_app->getUnitTree()->find(getValue("entrypoint"), m_attributes["basepath"], false));
		if (world_scene) {
			LOG(LOG_INFO) << "Entering world scene";
			if (m_parent && !m_parent->getTypeName().compare("//meworla/scene"))
				static_cast<Rhea::Scene*>(m_parent)->hide();
			world_scene->show();
		}
		else {
			LOG(LOG_ERROR) << "Avatars::onWorldEnter: no entrypoint scene defined.";
		}
		return false;
	}

	Rhea::Unit* Avatars::createType(std::string name, std::string type)
	{
		Rhea::Unit* ret = (Unit*)new Avatars(name, type);
		return ret;
	}
}
