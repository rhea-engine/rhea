/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#include <Rhea/Core/WorldService.h>

#include <Rhea/Application.h>
#include <Rhea/UnitTree.h>
#include <Rhea/Core/Item.h>
#include <Rhea/Core/ActiveUnit.h>
#include <Psio/Log.h>
#include "Rhea/Resources/Collection.h"

namespace rhea
{
	void WorldService::onInit()
	{
		m_app->getUnitTree()->registerEventHandler("scene_activate", this, [&](Event* ev, Unit* tree)->bool {
			onSceneActivate(doCast<Scene>(ev->getTarget()));
			return false;
			});
		m_app->getUnitTree()->registerEventHandler("scene_deactivate", this, [&](Event* ev, Unit* tree)->bool {
			onSceneDeactivate(doCast<Scene>(ev->getTarget()));
			return false;
			});
	}

	void WorldService::openWorld(std::string& rid, OpenHandler handler)
	{
		const uint64_t open_generation = ++m_open_generation;
		m_state = State::LoadingWorld;		

		m_app->loadResource(rid, "//meworla/world", [this, rid, open_generation, handler](Component* component, ReqError error)
			{
				if (open_generation != m_open_generation) {
					if (handler) {
						handler(OpenError::RequestCanceled);
					}
					return; // stale result
				}

				if (error == ReqError::OK) {
					LOG(LOG_INFO) << "Loaded world: " << rid;
					++m_session_generation;					
					onWorldLoaded(open_generation, rid, component, handler);
				}
				else {
					LOG(LOG_ERROR) << "Failed to load world: " << static_cast<int>(error) << " (" << rid << ")";
					m_state = State::Idle;
					if (handler) {
						handler(OpenError::ResourceNotFound);
					}
				}			
			});
	}

	void WorldService::loadScene(std::string resource_id, LoadSceneHandler handler)
	{
		// check if the scene is already loaded
		for (auto& pair : m_managed_scenes) {
			ManagedScene& managed_scene = pair.second;
			if (managed_scene.resource_id == resource_id && managed_scene.session_generation == m_session_generation) {
				if (handler) {
					handler(managed_scene.scene, OpenError::None);
				}
				return;
			}
		}
		SceneLoadKey key{ m_session_generation, resource_id };
		auto [it, inserted] = m_pending_scene_loads.try_emplace(key);
		if (handler) {
			it->second.handlers.push_back(handler);
		}

		if (!inserted)
			return; // Resource already being loaded, handler will be called when loading is complete

		const uint64_t session_generation = m_session_generation;
		m_app->loadResource(resource_id, "//meworla/scene", [this, resource_id, handler, session_generation](Component* component, ReqError error)
			{
				Scene* scene = doCast<Scene>(component);
				OpenError result = OpenError::None;

				if (session_generation != m_session_generation) {
					result = OpenError::RequestCanceled;
				}
				else {

					if (error == ReqError::OK) {
						LOG(LOG_DEBUG) << "Loaded scene: " << resource_id;

						if (scene) {
							ManagedScene& managed_scene = ensureManagedScene(scene, resource_id, SceneRole::Regular, nullptr);
							managed_scene.owned_by_worldservice = true;
						}
						else {
							LOG(LOG_ERROR) << "Loaded component is not a scene: " << resource_id;
							result = OpenError::InvalidSceneType;
						}
					}
					else {
						LOG(LOG_ERROR) << "Failed to load scene: " << static_cast<int>(error) << " (" << resource_id << ")";
						result = OpenError::ResourceNotFound;
					}
				}

				SceneLoadKey key{ session_generation, resource_id };
				auto it = m_pending_scene_loads.find(key);
				if (it == m_pending_scene_loads.end())
					return;

				auto pending = std::move(it->second);
				m_pending_scene_loads.erase(it);

				for (auto& handler : pending.handlers)
					handler(scene, result);
			});
	}
	
	void WorldService::activateScene(std::string& resource_id, SceneRole role, Scene* owner_scene, OpenHandler handler)
	{
		loadScene(resource_id, [this, resource_id, role, owner_scene, handler](Scene* next_scene, OpenError error) {
			if (error == OpenError::None) {				
				if (next_scene) {
					// prepare the scene if not already prepared
					ManagedScene& managed_scene = ensureManagedScene(next_scene, resource_id, role, owner_scene);
					m_managed_scenes[next_scene].role = role;
					m_managed_scenes[next_scene].owner_scene = owner_scene;
					if (!managed_scene.isPrepared()) {
						PrepareError prepareError = prepareScene(next_scene, managed_scene.role, managed_scene.owner_scene);
						if (prepareError != PrepareError::None) {
							if (handler) {
								handler(OpenError::InvalidSceneType);
							}
							return;
						}
						m_managed_scenes[next_scene].state = SceneLifecycleState::Prepared;
					}

					next_scene->activate();
					next_scene->show();

					if (handler) {
						handler(OpenError::None);
					}
				}
				else {
					LOG(LOG_ERROR) << "Activation failed: scene not found after loading: " << resource_id;					
					if (handler) {
						handler(OpenError::InvalidSceneType);
					}
				}
			}
			else {
				LOG(LOG_ERROR) << "Activation failed: could not load scene: " << resource_id;				
				if (handler) {
					handler(error);
				}
			}
			});
	}

	bool WorldService::deactivateScene(std::string& resource_id)
	{
		// find the loaded scene
		Scene* scene_to_deactivate = nullptr;
		for (auto& pair : m_managed_scenes) {
			ManagedScene& managed_scene = pair.second;
			if (managed_scene.resource_id == resource_id && managed_scene.session_generation == m_session_generation) {
				scene_to_deactivate = managed_scene.scene;
				break;
			}
		}
		if (scene_to_deactivate) {
			if (scene_to_deactivate == m_primary_scene) {
				LOG(LOG_WARNING) << "Cannot deactivate primary scene: " << resource_id;
				return false;
			}
			scene_to_deactivate->deactivate();
			scene_to_deactivate->hide();
			return true;
		}
		else {
			LOG(LOG_WARNING) << "Scene not found for deactivation: " << resource_id;
		}
		return false;
	}

	void rhea::WorldService::transitionToScene(const TransitionRequest request, OpenHandler handler)
	{
		loadScene(request.target_resource_id, [this, request, handler](Scene* next_scene, OpenError error) {
			if (error == OpenError::None) {
				// find the loaded scene
				if (next_scene) {					
					doTransitionScene(next_scene, &request, handler);
				}
				else {
					LOG(LOG_ERROR) << "Transition failed: scene not found after loading: " << request.target_resource_id;					
					if (handler) {
						handler(OpenError::InvalidSceneType);
					}
				}
			}
			else {
				LOG(LOG_ERROR) << "Transition failed: could not load scene: " << request.target_resource_id;				
				if (handler) {
					handler(error);
				}
			}
			});
	}

	bool WorldService::onFrame(float delta)
	{
		// update items in scenes that are active
		for (auto& pair : m_managed_scenes) {
			ManagedScene& managed_scene = pair.second;
			if (managed_scene.state == SceneLifecycleState::Active) {
				Scene* scene = managed_scene.scene;
				if (scene) {
					scene->traverse([delta](std::string& name, Component* unit)->bool {
						ActiveUnit* active_unit = doCast<ActiveUnit>(unit);
						if (active_unit) {
							active_unit->onFrame(delta);
						}
						return true;
						});
				}
			}
		}		
		return false;
	}

	bool WorldService::onFrameFinal(float delta)
	{
		return false;
	}

	void WorldService::onSceneActivate(Scene* scene)
	{
		// only add if not already in the list
		if (scene == nullptr)
			return;

		ManagedScene& mgmt_scr = ensureManagedScene(scene, "external", SceneRole::External, nullptr);
		mgmt_scr.state = SceneLifecycleState::Active;		
	}

	void WorldService::onSceneDeactivate(Scene* scene)
	{
		if (scene == nullptr)
			return;
		if (m_managed_scenes.find(scene) != m_managed_scenes.end()) {
			m_managed_scenes[scene].state = SceneLifecycleState::Prepared;
		}		
	}

	void WorldService::onWorldLoaded(uint64_t open_generation, std::string request_id, Component* component, OpenHandler handler)
	{
		// get start scene
		m_world = doCast<World>(component);
		if (m_world) {
			if (m_primary_scene) {
				m_primary_scene->hide();
				m_primary_scene->deactivate();
				m_primary_scene = nullptr;
			}
			if (prepareWorld(m_world) != PrepareError::None) {
				m_state = State::Idle;
				if (handler) {
					handler(OpenError::InvalidWorldType);
				}
				return;
			}
			loadStartScene(m_world, open_generation, handler);
		}
		else if (doCast<Scene>(component)) {
			// if the loaded component is a scene, we can use it directly
			LOG(LOG_INFO) << "Loaded scene: " << request_id;
			m_world = nullptr;
			Scene* scene = doCast<Scene>(component);
			if (prepareWorld(m_world, scene) != PrepareError::None) {
				m_state = State::Idle;
				if (handler) {
					handler(OpenError::InvalidWorldType);
				}
				return;
			}
			ManagedScene& managed_scene = ensureManagedScene(scene, request_id, SceneRole::Regular, nullptr);
			managed_scene.owned_by_worldservice = true;
			doTransitionScene(scene, nullptr, handler);
		}
		else if (doCast<Collection>(component)) {
			// user tried to open directory
			// try world.scene
			request_id += "/world.scene";
			openWorld(request_id, handler);
			return;
		}
		else
		{
			LOG(LOG_ERROR) << "Loaded scene is not a world or a scene: " << request_id;
			m_state = State::Idle;
			if (handler) {
				handler(OpenError::InvalidWorldType);
			}
		}

	}
	void WorldService::loadStartScene(World* world, uint64_t requestId, OpenHandler handler)
	{
		std::string start = m_world->getValueString("startscene", "");

		if (start.empty()) {
			LOG(LOG_ERROR) << "World has no startscene";
			if (handler) {
				handler(OpenError::InvalidStartScene);
			}
			return;
		}

		// first search within the already loaded world.scene.
		Scene* embedded = doCast<Scene>(m_world->find(start, m_world->getPath(), false));
		if (embedded) {
			ManagedScene& managed_scene = ensureManagedScene(embedded, m_world->getPath() + "/" + start, SceneRole::Regular, nullptr);
			managed_scene.owned_by_worldservice = true;
			doTransitionScene(embedded, nullptr, handler);
			return;
		}

		// Otherwise, load as an external resource ID.
		loadScene(start, [this, requestId, start,handler](Scene* next_scene, OpenError error) {
			
			if (requestId != m_open_generation) {
				if (handler)
					handler(OpenError::RequestCanceled);
				return;
			}

			if (error == OpenError::None) {
				if (next_scene) {
					doTransitionScene(next_scene, nullptr, handler);
				}
				else {
					LOG(LOG_ERROR) << "Start scene not found after loading: " << start;
					if (handler) {
						handler(OpenError::InvalidStartScene);
					}
				}
			}
			else {
				LOG(LOG_ERROR) << "Failed to load start scene: " << static_cast<int>(error) << " (" << start << ")";
				if (handler) {
					handler(error);
				}
			}
			});
	}

	void WorldService::doTransitionScene(Scene* scene, const TransitionRequest* request, OpenHandler handler)
	{	
		if (m_state == State::Transitioning) {
			if (handler)
				handler(OpenError::RequestCanceled);
			return;
		}

		if (m_managed_scenes.find(scene) == m_managed_scenes.end()) {
			LOG(LOG_ERROR) << "Scene is not managed by WorldService: " << scene->getPathName();
			if (handler) {
				handler(OpenError::InvalidSceneType);
			}
			return;
		}
		else if (scene == m_primary_scene) {
			LOG(LOG_WARNING) << "Scene is already the primary scene: " << scene->getPathName();
			if (handler) {
				handler(OpenError::None);
			}
			return;
		}
		else if (m_managed_scenes[scene].state == SceneLifecycleState::Loaded) {
			PrepareError prepareError = prepareScene(scene, SceneRole::Regular);
			if (prepareError != PrepareError::None) {
				if (handler) {
					handler(OpenError::InvalidSceneType);
				}
				return;
			}
			m_managed_scenes[scene].state = SceneLifecycleState::Prepared;
		}
		
		State previous_state = m_state;
		m_state = State::Transitioning;

		// prepare the transition to the new scene
		if (prepareSceneTransition(m_primary_scene, scene, nullptr, request) != PrepareError::None) {
			m_state = previous_state;
			if (handler) {
				handler(OpenError::InvalidStartScene);
			}
			return;
		}
		Scene* old_scene = m_primary_scene;
		if (m_primary_scene) {
			m_primary_scene->hide();
			m_primary_scene->deactivate();
			m_primary_scene = nullptr;
		}

		scene->activate();
		scene->show();

		m_primary_scene = scene;

		if (old_scene) {
			// if the old scene is not the primary scene anymore, we can deactivate it
			if (m_managed_scenes.find(old_scene) != m_managed_scenes.end()) {
				m_managed_scenes[old_scene].state = SceneLifecycleState::Prepared;
			}
		}

		m_state = State::Ready;
		m_managed_scenes[scene].state = SceneLifecycleState::Active;

		if (handler)
			handler(OpenError::None);
	}

	WorldService::PrepareError WorldService::prepareScene(Scene* next_scene, SceneRole role, Scene* owner_scene)
	{
		ScenePrepareContext context = { m_world, m_primary_scene, next_scene, role, owner_scene };
		for (ScenePrepareHandler& preparer : m_scene_prepare_handlers) {
			PrepareError error = preparer(&context);
			if (error != PrepareError::None)
				return error;
		}

		return PrepareError::None;
	}

	WorldService::PrepareError WorldService::prepareWorld(World* world,	Scene* standalone_scene)
	{
		WorldPrepareContext context = { world, standalone_scene };
		for (WorldPrepareHandler& preparer : m_world_prepare_handlers) {
			PrepareError error = preparer(&context);
			if (error != PrepareError::None)
				return error;
		}
		return PrepareError::None;
	}

	WorldService::PrepareError WorldService::prepareSceneTransition(Scene* source_scene, Scene* target_scene, Unit* entry_point, const TransitionRequest* request)
	{
		SceneTransitionContext context = { m_world, source_scene, target_scene, entry_point, request };
		for (TransitionHandler& handler : m_scene_transition_handlers) {
			TransitionError error = handler(&context);
			if (error != TransitionError::None)
				return PrepareError::InvalidController;
		}
		return PrepareError::None;
	}

	WorldService::ManagedScene& WorldService::ensureManagedScene(Scene* scene, std::string resourceId, SceneRole role, Scene* owner)
	{
		auto it = m_managed_scenes.find(scene);
		if (it != m_managed_scenes.end() && it->second.session_generation == m_session_generation) {
			return it->second;
		}
		else {
			ManagedScene managed_scene;
			managed_scene.scene = scene;
			managed_scene.resource_id = resourceId;
			managed_scene.role = role;
			managed_scene.owner_scene = owner;
			managed_scene.state = SceneLifecycleState::Loaded;
			managed_scene.session_generation = m_session_generation;
			m_managed_scenes[scene] = managed_scene;
			return m_managed_scenes[scene];
		}
	}
}