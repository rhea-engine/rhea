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
#include <Rhea/Components/ActiveComponent.h>
#include <Psio/Log.h>
#include "Rhea/Resources/Collection.h"
#include <Rhea/Core/SceneResource.h>
#include <memory>

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

		m_app->loadResource(rid, "//meworla/world", [this, rid, open_generation, handler](Component* component, ReqError error) {

			if (open_generation != m_open_generation) {
				if (handler) {
					handler(OpenError::RequestCanceled);
				}
				return; // stale result
			}

			SceneResource* scene_resource = doCast<SceneResource>(component);
			if (error == ReqError::OK && scene_resource) {
				const ResourceContext context{
					scene_resource->getContextRoot(),
					scene_resource->getSceneRoot()
				};
				// instance the world
				Unit* parent = doCast<Unit>(m_app->getUnitTree()->find(scene_resource->getSceneRoot()));
				scene_resource->spawn(parent, [this, rid, open_generation, handler, context](ReqError error, Unit* unit) {
					if (open_generation != m_open_generation) {
						if (handler) {
							handler(OpenError::RequestCanceled);
						}
						return; // stale result
					}
					if (error != ReqError::OK) {
						if (handler) {
							handler(OpenError::InvalidSceneType);
						}
						return;
					}

					World* world = doCast<World>(unit);
					if (world) {
						m_world = world;
						m_state = State::Ready;
					}
					LOG(LOG_INFO) << "Loaded world: " << rid;
					onWorldLoaded(open_generation, rid, context, unit, handler);
					});
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

	void WorldService::beginSession(const ResourceContext& context)
	{
		m_session_context = context;
		WorldSessionChangedEvent event(this, m_world, m_session_generation, m_session_generation + 1);

		++m_session_generation;
		deactivatePreviousSession();
		m_primary_scene = nullptr;

		sendEvent("world_session_changed", &event);
	}

	void WorldService::deactivatePreviousSession()
	{
		for (auto& [scene, managed] : m_managed_scenes) {
			if (managed.session_generation == m_session_generation)
				continue;

			if (managed.state == SceneLifecycleState::Active && managed.lifecycle_managed) {
				scene->hide();
				scene->deactivate();
			}
		}
	}
	void WorldService::loadScene(const std::string& resource_id, LoadSceneHandler handler)
	{
		loadScene(resource_id, m_session_context, handler);
	}

	void WorldService::loadScene(const std::string& resource_id, const ResourceContext& context, LoadSceneHandler handler)
	{
		if (resource_id.empty()) {
			LOG(LOG_ERROR) << "Invalid resource id: " << resource_id << " (empty)";
			if (handler) {
				handler(nullptr, OpenError::ResourceNotFound);
			}
			return;
		}
		std::string rid = context.normalizeResourceId(resource_id);

		// check if the scene is already loaded
		for (auto& pair : m_managed_scenes) {
			ManagedScene& managed_scene = pair.second;
			if (managed_scene.resource_id == rid && managed_scene.session_generation == m_session_generation) {
				if (handler) {
					handler(managed_scene.scene, OpenError::None);
				}
				return;
			}
		}
		SceneLoadKey key{ m_session_generation, rid };
		auto [it, inserted] = m_pending_scene_loads.try_emplace(key);
		if (handler) {
			it->second.handlers.push_back(handler);
		}

		if (!inserted)
			return; // Resource already being loaded, handler will be called when loading is complete

		const uint64_t session_generation = m_session_generation;
		m_app->loadResource(rid, context, "//meworla/scene", [this, rid, handler, session_generation](Component* component, ReqError error) {
			SceneResource* scene_resource = doCast<SceneResource>(component);

			OpenError result = OpenError::None;

			if (error != ReqError::OK || !scene_resource) {
				LOG(LOG_ERROR) << "Failed to load scene: " << static_cast<int>(error) << " (" << rid << ")";
				result = OpenError::ResourceNotFound;
			}

			Unit* parent = nullptr;
			if (result == OpenError::None) {
				parent = doCast<Unit>(m_app->getUnitTree()->find(scene_resource->getSceneRoot()));
				if (!parent) {
					LOG(LOG_ERROR) << "Failed to find scene root: " << scene_resource->getSceneRoot() << " (" << rid << ")";
					result = OpenError::InvalidSceneType;
				}
			}

			if (result != OpenError::None) {
				// Call all pending handlers with the error
				SceneLoadKey key{ session_generation, rid };
				auto it = m_pending_scene_loads.find(key);
				if (it == m_pending_scene_loads.end())
					return;

				auto pending = std::move(it->second);
				m_pending_scene_loads.erase(it);

				for (auto& handler : pending.handlers)
					handler(nullptr, result);
				return;
			}

			scene_resource->spawn(parent, [this, rid, session_generation](ReqError error, Unit* component) {

				Scene* scene = doCast<Scene>(component);
				OpenError result = OpenError::None;

				if (session_generation != m_session_generation) {
					result = OpenError::RequestCanceled;
				}
				else {

					if (error == ReqError::OK) {
						LOG(LOG_DEBUG) << "Loaded scene: " << rid;

						if (scene) {
							ManagedScene& managed_scene = ensureManagedScene(scene, rid, SceneRole::Regular, nullptr);
						}
						else {
							LOG(LOG_ERROR) << "Loaded component is not a scene: " << rid;
							result = OpenError::InvalidSceneType;
						}
					}
					else {
						LOG(LOG_ERROR) << "Failed to load scene: " << static_cast<int>(error) << " (" << rid << ")";
						result = OpenError::ResourceNotFound;
					}
				}

				if (result == OpenError::None) {

					// register all scene entry points
					scene->children([this, scene](std::string& name, Component* child) -> bool {
						if (auto* entry = doCast<SceneEntryPoint>(child)) {
							std::string entry_id = entry->getEntryId();
							ManagedScene& managed_scene = m_managed_scenes[scene];
							managed_scene.entry_points[entry_id] = entry;
						}
						return true;
						});
				}

				SceneLoadKey key{ session_generation, rid };
				auto it = m_pending_scene_loads.find(key);
				if (it == m_pending_scene_loads.end())
					return;

				auto pending = std::move(it->second);
				m_pending_scene_loads.erase(it);

				for (auto& handler : pending.handlers)
					handler(scene, result);
				});
			});
	}

	void WorldService::activateScene(const std::string& resource_id, SceneRole role, Scene* owner_scene, OpenHandler handler)
	{
		activateScene(resource_id, m_session_context, role, owner_scene, handler);
	}

	void WorldService::activateScene(const std::string& resource_id, const ResourceContext& context, SceneRole role, Scene* owner_scene, OpenHandler handler)
	{
		loadScene(resource_id, context, [this, resource_id, role, owner_scene, handler, context](Scene* next_scene, OpenError error) {
			if (error == OpenError::None) {
				if (next_scene) {
					// prepare the scene if not already prepared
					std::string rid = context.normalizeResourceId(resource_id);
					ManagedScene& managed_scene = ensureManagedScene(next_scene, rid, role, owner_scene);
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
					m_managed_scenes[next_scene].lifecycle_managed = true;

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

	bool WorldService::deactivateScene(const std::string& resource_id)
	{
		return deactivateScene(resource_id, m_session_context);
	}

	bool WorldService::deactivateScene(const std::string& resource_id, const ResourceContext& context)
	{
		// find the loaded scene
		std::string rid = context.normalizeResourceId(resource_id);
		Scene* scene_to_deactivate = nullptr;
		for (auto& pair : m_managed_scenes) {
			ManagedScene& managed_scene = pair.second;
			if (managed_scene.resource_id == rid && managed_scene.session_generation == m_session_generation) {
				scene_to_deactivate = managed_scene.scene;
				break;
			}
		}
		if (scene_to_deactivate) {
			if (scene_to_deactivate == m_primary_scene) {
				LOG(LOG_WARNING) << "Cannot deactivate primary scene: " << rid;
				return false;
			}
			scene_to_deactivate->deactivate();
			scene_to_deactivate->hide();
			return true;
		}
		else {
			LOG(LOG_WARNING) << "Scene not found for deactivation: " << rid;
		}
		return false;
	}

	void rhea::WorldService::transitionToScene(const TransitionRequest& request, OpenHandler handler)
	{
		loadScene(request.target_resource_id, m_session_context, [this, request, handler](Scene* next_scene, OpenError error) {
			if (error == OpenError::None) {
				// find the loaded scene
				if (next_scene) {
					doTransitionScene(next_scene, request, handler);
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
			if (managed_scene.state == SceneLifecycleState::Active && managed_scene.session_generation == m_session_generation) {
				Scene* scene = managed_scene.scene;
				if (scene) {
					scene->traverse([delta](std::string& name, Component* unit)->bool {
						ActiveUnit* active_unit = doCast<ActiveUnit>(unit);
						if (active_unit) {
							active_unit->onFrame(delta);
						}
						else if (doCast<ActiveComponent>(unit)) {
							doCast<ActiveComponent>(unit)->onFrame(delta);
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

	void WorldService::onWorldLoaded(uint64_t open_generation, std::string request_id, const ResourceContext& context, Component* component, OpenHandler handler)
	{
		// get start scene
		World* world = doCast<World>(component);
		if (world) {
			if (prepareWorld(world) != PrepareError::None) {
				m_state = m_primary_scene ? State::Ready : State::Idle;
				if (handler) {
					handler(OpenError::InvalidWorldType);
				}
				return;
			}
			beginSession(context);
			m_world = world;
			loadStartScene(world, context, open_generation, handler);
		}
		else if (doCast<Scene>(component)) {
			// if the loaded component is a scene, we can use it directly
			LOG(LOG_INFO) << "Loaded scene: " << request_id;
			Scene* scene = doCast<Scene>(component);
			if (prepareWorld(nullptr, scene) != PrepareError::None) {
				m_state = State::Idle;
				if (handler) {
					handler(OpenError::InvalidWorldType);
				}
				return;
			}
			beginSession(context);
			m_world = nullptr;

			ManagedScene& managed_scene = ensureManagedScene(scene, request_id, SceneRole::Regular, nullptr);
			managed_scene.lifecycle_managed = true;
			doTransitionScene(scene, TransitionRequest{}, handler);
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
	void WorldService::loadStartScene(World* world, const ResourceContext& context, uint64_t requestId, OpenHandler handler)
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
			managed_scene.lifecycle_managed = true;
			doTransitionScene(embedded, TransitionRequest{}, handler);
			return;
		}

		// Otherwise, load as an external resource ID.
		loadScene(start, context, [this, requestId, start, handler](Scene* next_scene, OpenError error) {

			if (requestId != m_open_generation) {
				if (handler)
					handler(OpenError::RequestCanceled);
				return;
			}

			if (error == OpenError::None) {
				if (next_scene) {
					doTransitionScene(next_scene, TransitionRequest{}, handler);
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

	void WorldService::doTransitionScene(Scene* scene, const TransitionRequest& request, OpenHandler handler)
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

		// get the entry point
		SceneEntryPoint* entry_point = nullptr;
		if (!request.entry_point.empty()) {
			auto it = m_managed_scenes[scene].entry_points.find(request.entry_point);
			if (it == m_managed_scenes[scene].entry_points.end()) {
				LOG(LOG_ERROR) << "Entry point not found in scene: " << request.entry_point;
				m_state = previous_state;
				if (handler) {
					handler(OpenError::EntryPointNotFound);
				}
				return;
			}
			entry_point = it->second;
		}
		else if (m_managed_scenes[scene].entry_points.find("default") != m_managed_scenes[scene].entry_points.end()) {
			// use the default entry point if no specific entry point is requested
			entry_point = m_managed_scenes[scene].entry_points["default"];
		}

		// prepare the transition to the new scene
		prepareSceneTransition(m_primary_scene, scene, entry_point, request, [this, scene, previous_state, handler](TransitionError error) {
			if (error != TransitionError::None) {
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
			m_managed_scenes[scene].lifecycle_managed = true;

			PrimarySceneChangedEvent event(this, m_primary_scene, old_scene, m_session_generation);
			sendEvent("primary_scene_changed", &event);

			if (handler)
				handler(OpenError::None);
			});
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

	WorldService::PrepareError WorldService::prepareWorld(World* world, Scene* standalone_scene)
	{
		WorldPrepareContext context = { world, standalone_scene };
		for (WorldPrepareHandler& preparer : m_world_prepare_handlers) {
			PrepareError error = preparer(&context);
			if (error != PrepareError::None)
				return error;
		}
		return PrepareError::None;
	}

	void WorldService::prepareSceneTransition(
		Scene* source_scene,
		Scene* target_scene,
		SceneEntryPoint* entry_point,
		const TransitionRequest& request,
		TransitionCompletion completion)
	{
		auto context = std::make_shared<SceneTransitionContext>(SceneTransitionContext{ m_world, source_scene, target_scene, entry_point, request });
		prepareSceneTransitionAsync(context, 0, [completion](TransitionError error) {
			completion(error);
			});
	}

	void WorldService::prepareSceneTransitionAsync(
		std::shared_ptr<SceneTransitionContext> context,
		size_t handlerIndex,
		TransitionCompletion complete)
	{
		if (handlerIndex >=
			m_scene_transition_handlers.size()) {
			complete(TransitionError::None);
			return;
		}

		auto& handler =
			m_scene_transition_handlers[handlerIndex];

		handler(
			context.get(),
			[this,
			context,
			handlerIndex,
			complete](TransitionError error) mutable {
				if (error != TransitionError::None) {
					complete(error);
					return;
				}

				prepareSceneTransitionAsync(
					context,
					handlerIndex + 1,
					std::move(complete));
			});
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