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

	void WorldService::loadScene(std::string& resource_id, OpenHandler handler)
	{
		// check if the scene is already loaded
		for (auto& pair : m_managed_scenes) {
			ManagedScene& managed_scene = pair.second;
			if (managed_scene.resource_id == resource_id) {
				LOG(LOG_INFO) << "Scene already loaded: " << resource_id;
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
					LOG(LOG_INFO) << "Loaded scene: " << m_request_path;
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
				for (auto& pair : m_managed_scenes) {
					ManagedScene& managed_scene = pair.second;
					if (managed_scene.resource_id == resource_id) {
						next_scene = managed_scene.scene;
						break;
					}
				}
				if (next_scene) {
					// prepare and activate the scene
					PrepareError prepareError = prepareScene(next_scene);
					if (prepareError != PrepareError::None) {
						if (handler) {
							handler(OpenError::InvalidStartScene);
						}
						return;
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

	void rhea::WorldService::transitionToScene(std::string& resource_id, OpenHandler handler)
	{
		loadScene(resource_id, [this, resource_id, handler](OpenError error) {
			if (error == OpenError::None) {
				// find the loaded scene
				Scene* next_scene = nullptr;
				for (auto& pair : m_managed_scenes) {
					ManagedScene& managed_scene = pair.second;
					if (managed_scene.resource_id == resource_id) {
						next_scene = managed_scene.scene;
						break;
					}
				}
				if (next_scene) {
					onSceneLoaded(next_scene, handler);
				}
				else {
					LOG(LOG_ERROR) << "Transition failed: scene not found after loading: " << resource_id;
					m_state = State::Failed;
					if (handler) {
						handler(OpenError::InvalidStartScene);
					}
				}
			}
			else {
				LOG(LOG_ERROR) << "Transition failed: could not load scene: " << resource_id;
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
			m_managed_scenes[scene].state = SceneLifecycleState::Loaded;
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
			onSceneLoaded(m_next_scene, handler);
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
			onSceneLoaded(m_next_scene, handler);
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
					onSceneLoaded(next_scene, handler);
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

	void WorldService::onSceneLoaded(Scene* scene, OpenHandler handler)
	{
		PrepareError prepareError = prepareScene(scene);
		if (prepareError != PrepareError::None) {
			if (handler) {
				handler(OpenError::InvalidStartScene);
			}
			return;
		}

		if (m_primary_scene) {
			m_primary_scene->hide();
			m_primary_scene->deactivate();
			m_primary_scene = nullptr;
		}

		m_primary_scene = scene;
		m_next_scene = nullptr;

		scene->activate();
		scene->show();
		m_state = State::Ready;
		if (handler)
			handler(OpenError::None);
	}

	WorldService::PrepareError WorldService::prepareScene(Scene* next_scene)
	{
		WorldContext context = { m_world, next_scene, m_primary_scene };
		for (PrepareHandler& preparer : m_prepare_handlers) {
			PrepareError error = preparer(&context);
			if (error != PrepareError::None)
				return error;
		}

		return PrepareError::None;
	}

	WorldService::PrepareError WorldService::prepareWorld(World* world)
	{
		WorldContext context = { world, m_next_scene, m_primary_scene };
		for (PrepareHandler& preparer : m_world_prepare_handlers) {
			PrepareError error = preparer(&context);
			if (error != PrepareError::None)
				return error;
		}
		return PrepareError::None;
	}
}