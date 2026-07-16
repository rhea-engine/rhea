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

		m_app->loadResource(
			start,
			"//meworla/scene",
			[this, requestId, handler](Component* component, ReqError error)
			{
				if (requestId != m_request_id)
					return;

				Scene* scene = doCast<Scene>(component);
				if (error != ReqError::OK || !scene) {
					LOG(LOG_ERROR) << "Could not load start scene: " << static_cast<int>(error) << " (" << m_request_path << ")";
					m_state = State::Failed;
					if (handler) {
						handler(OpenError::InvalidStartScene);
					}
					return;
				}				

				m_next_scene = scene;
				onSceneLoaded(m_next_scene, handler);
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