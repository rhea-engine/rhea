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
		const uint64_t request_id = ++m_request_id;
		m_state = State::LoadingWorld;
		m_request_path = rid;

		m_app->loadResource(rid, "//meworla/world", [this, request_id, handler](Component* component, ReqError error)
			{
				if (request_id != m_request_id) {
					if (handler) {
						handler(OpenError::RequestCanceled);
					}
					return; // stale result
				}

				if (error == ReqError::OK) {
					LOG(LOG_INFO) << "Loaded world: " << m_request_path;
					onWorldLoaded(request_id, component, handler);
				}
				else {
					LOG(LOG_ERROR) << "Failed to load world: " << static_cast<int>(error) << " (" << m_request_path << ")";
					m_state = State::Failed;
					if (handler) {
						handler(OpenError::ResourceNotFound);
					}
				}			
			});
	}

	void WorldService::loadScene(std::string resource_id, OpenHandler handler)
	{
		// check if the scene is already loaded
		for (auto& pair : m_managed_scenes) {
			ManagedScene& managed_scene = pair.second;
			if (managed_scene.resource_id == resource_id) {
				if (handler) {
					handler(OpenError::None);
				}
				return;
			}
		}

		const uint64_t request_id = ++m_request_id;
		m_state = State::LoadingStartScene;
		m_request_path = resource_id;
		m_app->loadResource(resource_id, "//meworla/scene", [this, request_id, handler](Component* component, ReqError error)
			{
				if (request_id != m_request_id) {
					if (handler) {
						handler(OpenError::RequestCanceled);
					}
					return; // stale result
				}
				if (error == ReqError::OK) {
					LOG(LOG_DEBUG) << "Loaded scene: " << m_request_path;
					Scene* scene = doCast<Scene>(component);
					if (scene) {
						m_managed_scenes[scene] = ManagedScene{ scene, SceneRole::External, SceneLifecycleState::Loaded, nullptr, m_request_path, true };
						if (handler) {
							handler(OpenError::None);
						}
					}
					else {
						LOG(LOG_ERROR) << "Loaded component is not a scene: " << m_request_path;
						m_state = State::Failed;
						if (handler) {
							handler(OpenError::InvalidStartScene);
						}
					}
				}
				else {
					LOG(LOG_ERROR) << "Failed to load scene: " << static_cast<int>(error) << " (" << m_request_path << ")";
					m_state = State::Failed;
					if (handler) {
						handler(OpenError::ResourceNotFound);
					}
				}
			});
	}
	
	void WorldService::activateScene(std::string& resource_id, OpenHandler handler)
	{
		loadScene(resource_id, [this, resource_id, handler](OpenError error) {
			if (error == OpenError::None) {
				// find the loaded scene
				Scene* next_scene = nullptr;
				ManagedScene managed_scene;
				for (auto& pair : m_managed_scenes) {
					managed_scene = pair.second;
					if (managed_scene.resource_id == resource_id) {
						next_scene = managed_scene.scene;
						break;
					}
				}
				if (next_scene) {
					// prepare the scene if not already prepared
					if (!managed_scene.isPrepared()) {
						PrepareError prepareError = prepareScene(next_scene, managed_scene.role, managed_scene.owner_scene);
						if (prepareError != PrepareError::None) {
							if (handler) {
								handler(OpenError::InvalidStartScene);
							}
							return;
						}
					}

					next_scene->activate();
					next_scene->show();

					m_next_scene = nullptr;
					if (handler) {
						handler(OpenError::None);
					}
				}
				else {
					LOG(LOG_ERROR) << "Activation failed: scene not found after loading: " << resource_id;
					m_state = State::Failed;
					if (handler) {
						handler(OpenError::InvalidStartScene);
					}
				}
			}
			else {
				LOG(LOG_ERROR) << "Activation failed: could not load scene: " << resource_id;
				m_state = State::Failed;
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
			if (managed_scene.resource_id == resource_id) {
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

	void rhea::WorldService::transitionToScene(const TransitionRequest* request, OpenHandler handler)
	{
		loadScene(request->target_resource_id, [this, request, handler](OpenError error) {
			if (error == OpenError::None) {
				// find the loaded scene
				Scene* next_scene = nullptr;
				for (auto& pair : m_managed_scenes) {
					ManagedScene& managed_scene = pair.second;
					if (managed_scene.resource_id == request->target_resource_id) {
						next_scene = managed_scene.scene;
						break;
					}
				}
				if (next_scene) {
					doTransitionScene(next_scene, const_cast<TransitionRequest*>(request), handler);
				}
				else {
					LOG(LOG_ERROR) << "Transition failed: scene not found after loading: " << request->target_resource_id;
					m_state = State::Failed;
					if (handler) {
						handler(OpenError::InvalidStartScene);
					}
				}
			}
			else {
				LOG(LOG_ERROR) << "Transition failed: could not load scene: " << request->target_resource_id;
				m_state = State::Failed;
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
		if (m_managed_scenes.find(scene) == m_managed_scenes.end()) {
			ManagedScene managed_scene;
			managed_scene.scene = scene;
			managed_scene.role = SceneRole::External;
			managed_scene.state = SceneLifecycleState::Active;
			m_managed_scenes[scene] = managed_scene;
		}
		else {
			m_managed_scenes[scene].state = SceneLifecycleState::Active;
		}		
	}

	void WorldService::onSceneDeactivate(Scene* scene)
	{
		if (scene == nullptr)
			return;
		if (m_managed_scenes.find(scene) != m_managed_scenes.end()) {
			m_managed_scenes[scene].state = SceneLifecycleState::Prepared;
		}		
	}

	void WorldService::onWorldLoaded(uint64_t requestId, Component* component, OpenHandler handler)
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
				m_state = State::Failed;
				if (handler) {
					handler(OpenError::InvalidWorldType);
				}
				return;
			}
			loadStartScene(requestId, handler);
		}
		else if (doCast<Scene>(component)) {
			// if the loaded component is a scene, we can use it directly
			LOG(LOG_INFO) << "Loaded scene: " << m_request_path;
			m_world = nullptr;
			m_next_scene = doCast<Scene>(component);
			if (prepareWorld(m_world) != PrepareError::None) {
				m_state = State::Failed;
				if (handler) {
					handler(OpenError::InvalidWorldType);
				}
				return;
			}
			doTransitionScene(m_next_scene, nullptr, handler);
		}
		else if (doCast<Collection>(component)) {
			// user tried to open directory
			// try world.scene
			m_request_path += "/world.scene";
			openWorld(m_request_path, handler);
			return;
		}
		else
		{
			LOG(LOG_ERROR) << "Loaded scene is not a world or a scene: " << m_request_path;
			m_state = State::Failed;
			if (handler) {
				handler(OpenError::InvalidWorldType);
			}
		}

	}
	void WorldService::loadStartScene(uint64_t requestId, OpenHandler handler)
	{
		std::string start = m_world->getValueString("startscene", "");

		if (start.empty()) {
			LOG(LOG_ERROR) << "World has no startscene";
			m_state = State::Failed;
			if (handler) {
				handler(OpenError::InvalidStartScene);
			}
			return;
		}

		// first search within the already loaded world.scene.
		Scene* embedded = doCast<Scene>(m_world->find(start, m_world->getPath(), false));
		if (embedded) {
			m_next_scene = embedded;
			doTransitionScene(m_next_scene, nullptr, handler);
			return;
		}

		// Otherwise, load as an external resource ID.
		m_state = State::LoadingStartScene;

		loadScene(start, [this, requestId, handler](OpenError error) {
			if (error == OpenError::None) {
				// find the loaded scene
				Scene* next_scene = nullptr;
				for (auto& pair : m_managed_scenes) {
					ManagedScene& managed_scene = pair.second;
					if (managed_scene.resource_id == m_request_path) {
						next_scene = managed_scene.scene;
						break;
					}
				}
				if (next_scene) {
					doTransitionScene(next_scene, nullptr, handler);
				}
				else {
					LOG(LOG_ERROR) << "Start scene not found after loading: " << m_request_path;
					m_state = State::Failed;
					if (handler) {
						handler(OpenError::InvalidStartScene);
					}
				}
			}
			else {
				LOG(LOG_ERROR) << "Failed to load start scene: " << static_cast<int>(error) << " (" << m_request_path << ")";
				m_state = State::Failed;
				if (handler) {
					handler(error);
				}
			}
			});
	}

	void WorldService::doTransitionScene(Scene* scene, TransitionRequest* request, OpenHandler handler)
	{		
		if (m_managed_scenes.find(scene) == m_managed_scenes.end()) {
			LOG(LOG_ERROR) << "Scene is not managed by WorldService: " << scene->getPathName();
			if (handler) {
				handler(OpenError::InvalidStartScene);
			}
			return;
		}
		else if (m_managed_scenes[scene].state == SceneLifecycleState::Active) {
			LOG(LOG_WARNING) << "Scene is already active: " << scene->getPathName();
			if (handler) {
				handler(OpenError::None);
			}
			return;
		}
		else if (m_managed_scenes[scene].state == SceneLifecycleState::Loaded) {
			PrepareError prepareError = prepareScene(scene, SceneRole::Primary);
			if (prepareError != PrepareError::None) {
				if (handler) {
					handler(OpenError::InvalidStartScene);
				}
				return;
			}
		}
		
		m_managed_scenes[scene].role = SceneRole::Primary;

		// prepare the transition to the new scene
		if (prepareSceneTransition(m_primary_scene, scene, nullptr, request) != PrepareError::None) {
			if (handler) {
				handler(OpenError::InvalidStartScene);
			}
			return;
		}
		if (m_primary_scene) {
			m_primary_scene->hide();
			m_primary_scene->deactivate();
			m_primary_scene = nullptr;
			if (m_managed_scenes.find(m_primary_scene) != m_managed_scenes.end())
				m_managed_scenes[m_primary_scene].state = SceneLifecycleState::Prepared;
		}

		m_primary_scene = scene;
		m_next_scene = nullptr;

		scene->activate();
		scene->show();
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

	WorldService::PrepareError WorldService::prepareWorld(World* world)
	{
		WorldPrepareContext context = { world, m_next_scene };
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
}