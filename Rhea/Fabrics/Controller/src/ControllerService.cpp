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
#include <Rhea/Core/SceneResource.h>
#include <algorithm>
#include <Rhea/UnitTree.h>

namespace rhea
{
	ControllerService::ControllerService(std::string name, std::string type_name) :
		Service(std::move(name), std::move(type_name))
	{}

	void ControllerService::onInit()
	{
		auto* worlds = doCast<WorldService>(m_app->getService("world"));
		worlds->addWorldPrepareHandler([this, worlds](rhea::WorldService::WorldPrepareContext* context) -> rhea::Error
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
					return rhea::Error(rhea::Error::Code::UNAVAILABLE, "ControllerService: World not found");
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
								int slot = ctrl->getValue("player_slot", 0);
								ctrl->onActivate(); // Todo - only for local controllers, remote controllers should be activated when connected
								m_controllers.push_back({ ctrl, slot, m_next_controller_id++ });
							}
							return true;
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
						PlayerSlotHandle slot = ((int)ctrl->getValue("player_slot", (int)InvalidPlayerSlot));
						ctrl->onActivate(); // Todo - only for local controllers, remote controllers should be activated when connected
						m_controllers.push_back({ ctrl, slot, m_next_controller_id++ });
					}
				}
				if (m_controllers.empty()) {
					LOG(LOG_INFO) << "ControllerService: No controller found in world " << context->world->getPathName();
					return rhea::Error();
				}
				return rhea::Error();
			});

		worlds->addSceneTransitionHandler([this, worlds](rhea::WorldService::SceneTransitionContext* context, rhea::WorldService::TransitionCompletion completion) -> void
			{
				// find by player slot
				for (auto& ctrl_info : m_controllers) {
					if (!ctrl_info.controller)
						continue;

					// first phase search for character and camera

					// first subphase: iterate over all items in the scene to find all items with the same player_slot as the controller
					Item* character = nullptr;
					Camera* camera = nullptr;
					for (auto& item : context->target_scene->getItems()) {
						int controller_id = item->getValue("__controller_id", -1);

						if (controller_id != -1 && controller_id == ctrl_info.controller_id) {
							if (auto cam = doCast<rhea::Camera>(item)) {
								camera = cam;
							}
							else
								character = item;
							continue;
						}

						PlayerSlotHandle char_slot = ((int)item->getValue("player_slot", (int)InvalidPlayerSlot));
						if (char_slot == InvalidPlayerSlot)
							continue;

						if (char_slot != ctrl_info.player_slot)
							continue;
						if (auto cam = doCast<rhea::Camera>(item)) {
							camera = cam;
						}
						else
							character = item;
					}

					// second subphase: if not found, try to use scene-definition or template character from the world
					if (!character) {
						Ref<SceneResource> default_character_ref = ctrl_info.controller->getDefaultCharacter();
						if (default_character_ref) {
							SceneResource* default_character = m_app->getUnitTree()->resolve<SceneResource>(default_character_ref);
							if (default_character) {
								ErrorReport report;
								character = doCast<Item>(default_character->spawnPrepared(context->target_scene, "", &report));
								if (character) {
									LOG(LOG_INFO) << "ControllerService: Default character spawned: " << default_character_ref.id();
								}
								else
									LOG(LOG_ERROR) << "ControllerService: Default character failed to spawn: " << default_character_ref.id();
							}
							else
								LOG(LOG_ERROR) << "ControllerService: Default character not found: " << default_character_ref.id();
						}
					}
					if (!character) {
						std::string template_character_path = ctrl_info.controller->getValueString("default_character_template", "");
						if (!template_character_path.empty()) {
							Item* template_character = doCast<Item>(context->world->find(template_character_path));
							if (template_character) {
								character = doCast<Item>(template_character->instance(template_character->getPathName()));
								if (character) {
									context->target_scene->addChild(character);
									character->ready();
								}
							}
							else
								LOG(LOG_ERROR) << "ControllerService: Default character template not found: " << template_character_path;
						}
					}
					if (!camera) {
						std::string template_camera_path = ctrl_info.controller->getValueString("default_camera_template", "");
						if (!template_camera_path.empty()) {
							Camera* template_camera = doCast<Camera>(context->world->find(template_camera_path));
							if (template_camera) {
								camera = doCast<Camera>(template_camera->instance(template_camera->getPathName()));
								if (camera) {
									context->target_scene->addChild(camera);
									camera->ready();
								}
							}
						}
					}

					// third subphase: if not found, try to use class
					// TODO


					// second phase register the controller to the character and camera

					// setup new position and rotation for the character
					 
					rhea::Item* source_character = ctrl_info.controller->getTarget();
		

					// iterate over placement handlers by priority
					PlacementResult placement = PlacementResult::Unhandled;
					for (const auto& slot : m_placement_handlers) {
						PlacementContext placement_context;
						placement_context.transition = context;
						placement_context.player_slot = ctrl_info.player_slot;
						placement_context.controller = ctrl_info.controller;
						placement_context.source_character = source_character;
						placement_context.target_character = character;

						placement = slot.handler(placement_context);
						if (placement == PlacementResult::Placed || placement == PlacementResult::PlacedKeepInput) {
							break;
						}
						if (placement == PlacementResult::Failed) {
							LOG(LOG_WARNING) << "ControllerService: Placement handler failed for player_slot " << ctrl_info.player_slot;
							completion(rhea::Error(rhea::Error::Code::UNAVAILABLE, "ControllerService: Placement handler failed for player_slot " + std::to_string(ctrl_info.player_slot)));
							break;
						}
					}
					if (placement == PlacementResult::Unhandled && context->entry_point) {
						// if no placement handler handled the placement, use the entry point as fallback
						if (character) {
							auto position = context->entry_point->getTransform()->getWorldPosition();
							character->getTransform()->setWorldPosition(position);
						}
					}

					if (character) {
						//if a character or camera was found, switch the controller to it, this disconnects the old controller
						character->setValue("__controller_id", ctrl_info.controller_id);
						ctrl_info.controller->switchTarget(character, (placement != PlacementResult::KeepInput && placement != PlacementResult::PlacedKeepInput));
					}
					else
						LOG(LOG_WARNING) << "ControllerService: No character found in target scene " << context->target_scene->getPathName() << " for player_slot " << ctrl_info.player_slot;

					auto* spectator_ctrl = doCast<SpectatorController>(ctrl_info.controller);
					if (spectator_ctrl) {
						spectator_ctrl->setCamera(nullptr);
						if (camera) {
							camera->setValue("__controller_id", ctrl_info.controller_id);
							spectator_ctrl->setCamera(camera);
						}
					}
				}
				completion(rhea::Error());
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

	Controller* ControllerService::getPrimaryController() const
	{
		if(m_controllers.size() == 1)
			return m_controllers.front().controller;

		for (const ControllerBinding& binding : m_controllers) {
			if (binding.controller && binding.controller->getValue("primary", false).toBool()) {
				return binding.controller;
			}
		}

		return nullptr;
	}

	Controller* ControllerService::getController(PlayerSlotHandle player_slot) const
	{
		for (const ControllerBinding& binding : m_controllers) {
			if (binding.player_slot == player_slot)
				return binding.controller;
		}

		return nullptr;
	}

	void ControllerService::addPlacementHandler(PlacementHandler handler, int priority)
	{
		m_placement_handlers.push_back({std::move(handler), priority}); 
		// resort by priority, highest first
		std::sort(m_placement_handlers.begin(), m_placement_handlers.end(),
			[](const auto& a, const auto& b) {
				return a.priority > b.priority;
			});
	}
}