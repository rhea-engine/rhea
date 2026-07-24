/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */
#include <Rhea/Controller/ControllerService.h>
#include <Rhea/Core/WorldService.h>
#include <Psio/Log.h>
#include <Rhea/Core/Item.h>
#include <Rhea/Core/Camera.h>
#include <Rhea/Controller/SpectatorController.h>

namespace rhea
{
	ControllerService::ControllerService(std::string name, std::string type_name) :
		Service(std::move(name), std::move(type_name))
	{}

	void ControllerService::onInit()
	{
		auto* worlds = doCast<WorldService>(m_app->getService("world"));
		worlds->addWorldPrepareHandler([this, worlds](rhea::WorldService::WorldPrepareContext* context) -> rhea::WorldService::PrepareError
			{
				// deactivate old controllers
				for (auto& ctrl_info : m_controllers) {
					if (ctrl_info.controller) {
						ctrl_info.controller->disconnect();
						ctrl_info.controller->onDeactivate();
					}
				}
				m_controllers.clear();

				rhea::Controller* ctrl = nullptr;
				if (!context->world) {
					LOG(LOG_ERROR) << "ControllerService: World not found";
					return rhea::WorldService::PrepareError::InvalidController;
				}
				// get controller folder from the world, if it exists
				std::string controller_folder = context->world->getValueString("controllers", "");
				if (!controller_folder.empty()) {
					
					// find the first controller in the folder
					Unit* folder = doCast<Unit>(context->world->find(controller_folder));
					if (folder) {
						// iterate over children if type is controller register it
						folder->children([this](std::string& name, Component* child)->bool {
							rhea::Controller* ctrl = doCast<rhea::Controller>(child);
							if (ctrl) {
								int slot = ctrl->getValue("player_slot", -1);
								ctrl->onActivate(); // Todo - only for local controllers, remote controllers should be activated when connected
								m_controllers.push_back({ ctrl, static_cast<unsigned>(slot), nullptr });
							}
							return false;
							});
					}
					else {
						LOG(LOG_WARNING) << "ControllerService: Controller folder not found: " << controller_folder;
					}
				}
				else {
					// if no controller folder is specified, find the first controller in the world
					// NOTE: for now we just find the first one
					ctrl = context->world->findByType<rhea::Controller>();
					if (ctrl) {
						int slot = ctrl->getValue("player_slot", -1);
						ctrl->onActivate(); // Todo - only for local controllers, remote controllers should be activated when connected
						m_controllers.push_back({ ctrl, static_cast<unsigned>(slot), nullptr });
					}
				}
				if (m_controllers.empty()) {
					LOG(LOG_ERROR) << "ControllerService: No controller found in world " << context->world->getPathName();
					return rhea::WorldService::PrepareError::InvalidController;
				}
				return rhea::WorldService::PrepareError::None;
			});

		worlds->addSceneTransitionHandler([this, worlds](rhea::WorldService::SceneTransitionContext* context) -> rhea::WorldService::TransitionError
			{
				// TODO: disconnect from target in old scene?
				for (auto& ctrl_info : m_controllers) {
					if (ctrl_info.controller) {
						// iterate over all items in the scene to find all items with the same player_slot as the controller
						Item* character = nullptr;
						for (auto& item : context->target_scene->getItems()) {
							int char_slot = item->getValue("player_slot", -1);

							if (char_slot != static_cast<int>(ctrl_info.player_slot))
								continue;

							character = item;
						}
						if (character) {
							ctrl_info.controller->disconnect();
							ctrl_info.controller->connect(character);
							// find the camera
							if (auto spectator_ctrl = doCast<SpectatorController>(ctrl_info.controller)) {
								rhea::Camera* cam = context->target_scene->findByType<rhea::Camera>();
								if (!cam) {
									cam = new rhea::Camera("scene_view");
									cam->setup(m_app, {}, {});
								}
								spectator_ctrl->setCamera(cam);
								context->target_scene->addChild(cam);
								cam->ready();
							}
						}
						else
							LOG(LOG_WARNING) << "ControllerService: No character found in target scene " << context->target_scene->getPathName() << " for player_slot " << ctrl_info.player_slot;
					}
				}
				return rhea::WorldService::TransitionError::None;
			});
	}

	bool ControllerService::onFrame(float delta)
	{
		// update all controllers
		for (auto& ctrl_info : m_controllers) {
			if (ctrl_info.controller) {
				ctrl_info.controller->onFrame(delta);
			}
		}
		return false;
	}
}