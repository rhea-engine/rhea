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
		worlds->addWorldPrepareHandler([this, worlds](rhea::WorldService::WorldPrepareContext* context, rhea::WorldService::PrepareCompletion completion)->void
			{
				onWorldPrepare(context, completion);
			}
		);

		worlds->addWorldActivateHandler([this, worlds](rhea::WorldService::WorldPrepareContext* context) -> void
			{
				onWorldActivate(context);
			}
		);
	
		worlds->addSceneTransitionPrepareHandler([this, worlds](rhea::WorldService::SceneTransitionContext* context, rhea::WorldService::PrepareCompletion completion) -> void
			{
				onSceneTransitionPrepare(context, completion);
			}
		);

		worlds->addSceneTransitionActivateHandler([this, worlds](rhea::WorldService::SceneTransitionContext* context) -> void
			{
				onSceneTransitionActivate(context);
			}
		);

		worlds->addDiscardHandler([this, worlds](uint64_t generation) -> void
			{
				onDiscard(generation);
			}
		);
	}

	void ControllerService::onWorldPrepare(rhea::WorldService::WorldPrepareContext* context, rhea::WorldService::PrepareCompletion completion)
	{
		m_prepared_controllers = {};
		m_prepared_controllers.generation = context->open_generation;

		rhea::Controller* ctrl = nullptr;
		if (!context->world && !context->standalone_scene) {
			LOG(LOG_ERROR) << "ControllerService: no world or standalone scene found";
			completion(rhea::Error(rhea::Error::Code::UNAVAILABLE, "ControllerService: World or standalone scene not found"));
			return;
		}

		Scene* root_scene = context->world ? context->world : context->standalone_scene;
		m_prepared_controllers.root = root_scene;

		// get controller folder from the world, if it exists
		std::string controller_folder = root_scene->getValueString("controllers", "");
		if (!controller_folder.empty()) {

			// find the first controller in the folder
			Unit* folder = doCast<Unit>(root_scene->find(controller_folder));
			if (folder) {
				// iterate over children if type is controller register it
				folder->children([this, completion](std::string& name, Component* child)->bool {
					rhea::Controller* ctrl = doCast<rhea::Controller>(child);
					if (ctrl) {
						int slot = ctrl->getValue("player_slot", 0);						
						m_prepared_controllers.bindings.push_back({ ctrl, slot, ++m_prepared_controllers.next_controller_id });
					}

					return true;
					});
				completion(rhea::Error());
				return;
			}
			else {
				LOG(LOG_WARNING) << "ControllerService: Controller folder not found: " << controller_folder;
			}
		}
		else {
			// if no controller folder is specified, find the first controller in the world
			// NOTE: for now we just find the first one
			ctrl = root_scene->findByType<rhea::Controller>();
			if (ctrl) {
				PlayerSlotHandle slot = ((int)ctrl->getValue("player_slot", (int)InvalidPlayerSlot));				
				m_prepared_controllers.bindings.push_back({ ctrl, slot, ++m_prepared_controllers.next_controller_id });
			}
		}
		if (m_prepared_controllers.bindings.empty()) {
			LOG(LOG_INFO) << "ControllerService: No controller found in world " << root_scene->getPathName();
			completion(rhea::Error());
			return;
		}
		completion(rhea::Error());
	}

	void ControllerService::onWorldActivate(rhea::WorldService::WorldPrepareContext* context)
	{
		// deactivate all controllers from the previous world
		for (auto& ctrl_info : m_controllers) {
			if (!ctrl_info.controller)
				continue;
			ctrl_info.controller->disconnect();
			ctrl_info.controller->onDeactivate();
		}

		// activate the prepared controllers
		for (auto& ctrl_info : m_prepared_controllers.bindings) {
			if (!ctrl_info.controller)
				continue;
			ctrl_info.controller->onActivate(); // Todo - only for local controllers, remote controllers should be activated when connected
		}
		m_controllers = m_prepared_controllers.bindings;
	}
	
	void ControllerService::onSceneTransitionPrepare(rhea::WorldService::SceneTransitionContext* context, rhea::WorldService::PrepareCompletion completion)
	{
		auto controllers = m_controllers;
		Scene* root_scene = context->world ? context->world : context->target_scene;

		if (context->starts_new_session) {
			if (m_prepared_controllers.generation != context->open_generation) {
				LOG(LOG_WARNING) << "ControllerService: Prepared controllers generation mismatch. Expected " << context->open_generation << ", got " << m_prepared_controllers.generation;
				completion(rhea::Error(rhea::Error::Code::CANCELED, "ControllerService: Prepared controllers generation mismatch"));
				return;
			}
			controllers = m_prepared_controllers.bindings;
			root_scene = m_prepared_controllers.root;
		}

		PreparedTransition prepared;
		prepared.generation = context->open_generation;

		auto finish_prepare = [&](ErrorReport report) {
			m_prepared_transition = std::move(prepared);
			completion(std::move(report));
			};

		// find by player slot
		for (auto& ctrl_info : controllers) {
			if (!ctrl_info.controller)
				continue;

			// first phase search for character and camera

			// first subphase: iterate over all items in the scene to find all items with the same player_slot as the controller
			Item* character = nullptr;
			Camera* camera = nullptr;
			bool character_is_candidate = false;
			bool camera_is_candidate = false;

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
							character->ready();
							LOG(LOG_INFO) << "ControllerService: Default character spawned: " << default_character_ref.id();
							character_is_candidate = true;
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
				if (!template_character_path.empty() && root_scene) {					
					Item* template_character = doCast<Item>(root_scene->find(template_character_path));
					if (template_character) {
						character = doCast<Item>(template_character->instance(template_character->getPathName()));
						if (character) {
							LOG(LOG_INFO) << "ControllerService: Default character template instantiated: " << template_character_path;
							character_is_candidate = true;
							if (!context->target_scene->addChild(character)) {
								LOG(LOG_ERROR) << "ControllerService: Failed to add character to target scene: " << character->getPathName();
								character->destroy();
								character = nullptr;
							} else {
								character->ready();
							}
						}
						else
							LOG(LOG_ERROR) << "ControllerService: Default character template failed to instantiate: " << template_character_path;
					}
					else
						LOG(LOG_ERROR) << "ControllerService: Default character template not found: " << template_character_path;
				}
			}
			if (!camera) {
				std::string template_camera_path = ctrl_info.controller->getValueString("default_camera_template", "");
				if (!template_camera_path.empty() && root_scene) {					
					Camera* template_camera = doCast<Camera>(root_scene->find(template_camera_path));
					if (template_camera) {
						camera = doCast<Camera>(template_camera->instance(template_camera->getPathName()));
						if (camera) {
							LOG(LOG_INFO) << "ControllerService: Default camera template instantiated: " << template_camera_path;
							camera_is_candidate = true;
							if (!context->target_scene->addChild(camera)) {
								LOG(LOG_ERROR) << "ControllerService: Failed to add camera to target scene: " << camera->getPathName();
								camera->destroy();
								camera = nullptr;
							} else {
								camera->ready();
							}
						}
						else
							LOG(LOG_ERROR) << "ControllerService: Default camera template failed to instantiate: " << template_camera_path;
					}
				}
			}

			// third subphase: if not found, try to use class
			// TODO


			// second phase register the controller to the character and camera

			// setup new position and rotation for the character

			rhea::Item* source_character = ctrl_info.controller->getTarget();

			prepared.bindings.push_back({
				ctrl_info.controller, character, character_is_candidate, camera, camera_is_candidate,
				ctrl_info.controller_id, false
				});

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
					finish_prepare(rhea::Error(rhea::Error::Code::UNAVAILABLE, "ControllerService: Placement handler failed for player_slot " + std::to_string(ctrl_info.player_slot)));
					return;
				}
			}
			if (placement == PlacementResult::Unhandled && context->entry_point) {
				// if no placement handler handled the placement, use the entry point as fallback
				if (character) {
					auto position = context->entry_point->getTransform()->getWorldPosition();
					character->getTransform()->setWorldPosition(position);
				}
			}

			if (!character) 
				LOG(LOG_WARNING) << "ControllerService: No character found in target scene " << context->target_scene->getPathName() << " for player_slot " << ctrl_info.player_slot;			

			const bool reset_input =
				placement != PlacementResult::KeepInput &&
				placement != PlacementResult::PlacedKeepInput;

			prepared.bindings.back().reset_input = reset_input;

		}
		finish_prepare({});
	}

	void ControllerService::onSceneTransitionActivate(rhea::WorldService::SceneTransitionContext* context)
	{
		if (m_prepared_transition.generation != context->open_generation) {
			LOG(LOG_WARNING) << "ControllerService: no bindings for this transition";
			return;
		}

		auto prepared = std::exchange(m_prepared_transition, {});

		for (auto& binding : prepared.bindings) {
			if (binding.character) {
				binding.character->setValue("__controller_id", binding.controller_id);
				binding.controller->switchTarget(binding.character, binding.reset_input);
			}

			if (auto* spectator = doCast<SpectatorController>(binding.controller)) {
				if (binding.camera)
					binding.camera->setValue("__controller_id", binding.controller_id);

				spectator->setCamera(binding.camera);
			}
		}
	}

	void ControllerService::onDiscard(uint64_t generation)
	{
		if (m_prepared_controllers.generation == generation) {			
			m_prepared_controllers = {};
		}
		if (m_prepared_transition.generation == generation) {
			for (auto& binding : m_prepared_transition.bindings) {
				if (binding.character && binding.character_is_candidate) {
					binding.character->destroy();
				}
				if (binding.camera && binding.camera_is_candidate) {
					binding.camera->destroy();
				}
			}
			m_prepared_transition = {};
		}
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