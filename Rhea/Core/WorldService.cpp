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
#include <Rhea/Core/Async.h>

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

	void WorldService::openWorld(const std::string& rid, OpenHandler handler)
	{
		if (m_open_attempt.generation != 0) {
			if (handler) {
				handler(Error(Error::Code::UNAVAILABLE, "A world or scene operation is already in progress"));
			}
			return;
		}

		m_open_attempt = {
			++m_open_generation,
			++m_next_session_generation,
			nullptr,
			false
		};
		loadWorld(rid, m_open_attempt, handler);
	}

	void WorldService::loadWorld(const std::string& rid, OpenAttempt open_attempt, OpenHandler handler, bool allow_collection)
	{
		m_app->loadResource(rid, "//meworla/world", [this, rid, open_attempt, handler, allow_collection](Component* component, ErrorReport report) {
			if (open_attempt != m_open_attempt) {
				report.addError(Error(Error::Code::CANCELED, "Request canceled"));
				finishAttempt(open_attempt, std::move(report), handler);
				return; // stale result
			}

			SceneResource* scene_resource = doCast<SceneResource>(component);
			if (!report.isError() && scene_resource) {
				const auto& source_id = scene_resource->getSourceResourceId();

				// already the active scene? then just finish the attempt
				if (m_primary_scene && !source_id.empty() && source_id == m_active_root_resource_id) {
					finishAttempt(open_attempt, std::move(report), handler);
					return;
				}
				m_open_attempt.resource_id = source_id;
				const ResourceContext context{
					scene_resource->getContextRoot(),
					scene_resource->getSceneRoot(),
				};
				// instance the world
				Unit* parent = doCast<Unit>(m_app->getUnitTree()->find(scene_resource->getBaseId()));
				scene_resource->spawn(parent, [this, rid, open_attempt, report, handler, context](ErrorReport result, Unit* unit) mutable {
					report.merge(std::move(result));
					if (open_attempt != m_open_attempt) {
						if (unit)
							unit->destroy();
						report.addError(Error(Error::Code::CANCELED, "Request canceled"));
						finishAttempt(open_attempt, std::move(report), handler);
						return; // stale result
					}
					if (report.isError()) {
						finishAttempt(open_attempt, std::move(report), handler);
						return;
					}

					World* world = doCast<World>(unit);
					m_open_attempt.candidate_root = unit;
					LOG(LOG_INFO) << "Loaded world: " << rid;
					onWorldLoaded(m_open_attempt, rid, context, report, unit, handler);
					});
			}
			else if (!report.isError() && doCast<Collection>(component) && allow_collection) {
				// user tried to open directory
				// try world.scene				
				auto request_id = rid;
				if (!request_id.empty() && request_id.back() == '/')
					request_id.pop_back();
				request_id += "/world.scene";
				loadWorld(request_id, open_attempt, handler, false);
				return;
			}
			else {
				if (!report.isError() && !scene_resource) {
					LOG(LOG_ERROR) << "Failed to load world: Resource is not a scene resource: " << rid;
					report.addError(Error(Error::Code::UNSUPPORTED_FORMAT, "Resource is not a scene resource", rid));
				}
				else {
					LOG(LOG_ERROR) << "Failed to load world: " << rid;
				}
				finishAttempt(open_attempt, std::move(report), handler);
			}
			});
	}

	void WorldService::beginSession(const ResourceContext& context, uint64_t next_session_generation)
	{
		if (m_session_generation)
			deactivateSession(m_session_generation);

		m_session_generation = next_session_generation;
		m_session_context = context;
		m_primary_scene = nullptr;
	}

	void WorldService::deactivateSession(uint64_t session_generation)
	{
		for (auto& [scene, managed] : m_managed_scenes) {
			if (managed.session_generation != session_generation)
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
		if (m_open_attempt.generation != 0) {
			if (handler) {
				handler(nullptr, Error(Error::Code::UNAVAILABLE, "A world or scene operation is already in progress"));
			}
			return;
		}
		m_open_attempt = {
			++m_open_generation,
			m_session_generation,
			nullptr,
			nullptr,
			false
		};
		doLoadScene(resource_id, m_world, m_open_attempt, context, [this, handler, attempt = m_open_attempt](Scene* scene, ErrorReport report) mutable {
			finishAttempt(attempt, std::move(report), [handler, scene](ErrorReport report) {
				if (handler) {
					if (report.isError()) {
						handler(nullptr, std::move(report));
					}
					else {
						handler(scene, std::move(report));
					}
				}
				});
			});
	}
		
	void WorldService::doLoadScene(const std::string & resource_id, World* world, OpenAttempt open_attempt, 
		const ResourceContext & context, LoadSceneHandler handler)
	{
		ErrorReport report{};
		if (resource_id.empty()) {
			LOG(LOG_ERROR) << "Invalid resource id: " << resource_id << " (empty)";
			if (handler) {
				report.addError(Error(Error::Code::INVALID_ARGUMENT, "Resource id is empty"));
				handler(nullptr, std::move(report));
			}
			return;
		}
		std::string rid;
		if (!context.normalizeResourceId(resource_id, rid)) {
			LOG(LOG_ERROR) << "Failed to normalize resource id: " << resource_id;
			report.addError(Error(Error::Code::INVALID_DATA, "Failed to normalize resource address: " + rid, resource_id));
			if (handler) {
				handler(nullptr, std::move(report));
			}
			return;
		}

		// check if the scene is already loaded
		for (auto& pair : m_managed_scenes) {
			ManagedScene& managed_scene = pair.second;
			if (managed_scene.resource_id == rid && managed_scene.session_generation == open_attempt.session &&
				(managed_scene.candidate_generation == 0 ||	managed_scene.candidate_generation == open_attempt.generation)) 
			{
				if (handler) {
					handler(managed_scene.scene, report);
				}
				return;
			}
		}
		SceneLoadKey key{ open_attempt.generation, open_attempt.session, rid };
		auto [it, inserted] = m_pending_scene_loads.try_emplace(key);
		if (handler) {
			it->second.handlers.push_back(handler);
		}

		if (!inserted)
			return; // Resource already being loaded, handler will be called when loading is complete

		m_app->loadResource(rid, context, "//meworla/scene", [this, rid, handler, open_attempt, report](Component* component, ErrorReport result) mutable {
			SceneResource* scene_resource = doCast<SceneResource>(component);

			report.merge(std::move(result));

			if (report.isError()) {
				LOG(LOG_ERROR) << "Failed to load scene: " << rid;
			}
			else if (!scene_resource) {
				LOG(LOG_ERROR) << "Resource is not a scene resource: " << rid;
				report.addError(Error(Error::Code::UNSUPPORTED_FORMAT, "Resource is not a scene resource", rid));
			}

			Unit* parent = nullptr;
			if (!report.isError()) {
				parent = doCast<Unit>(m_app->getUnitTree()->find(scene_resource->getBaseId()));
				if (!parent) {
					LOG(LOG_ERROR) << "Failed to find scene root: " << scene_resource->	getBaseId() << " (" << rid << ")";
					report.addError(Error(Error::Code::INVALID_DEFINITION, "Invalid scene type", rid));
				}
			}

			if (report.isError()) {
				// Call all pending handlers with the error
				SceneLoadKey key{ open_attempt.generation, open_attempt.session, rid };
				auto it = m_pending_scene_loads.find(key);
				if (it == m_pending_scene_loads.end())
					return;

				auto pending = std::move(it->second);
				m_pending_scene_loads.erase(it);

				for (auto& handler : pending.handlers)
					handler(nullptr, report);
				return;
			}

			scene_resource->spawn(parent, [this, rid, report, open_attempt](ErrorReport result, Unit* component) mutable {

				auto finishLoad = [this, open_attempt](Scene* scene, const std::string& rid, ErrorReport report) {
					SceneLoadKey key{ open_attempt.generation, open_attempt.session, rid };
					auto it = m_pending_scene_loads.find(key);
					if (it == m_pending_scene_loads.end())
						return;

					auto pending = std::move(it->second);
					m_pending_scene_loads.erase(it);

					for (auto& handler : pending.handlers)
						handler(scene, report);
					};

				report.merge(std::move(result));

				Scene* scene = doCast<Scene>(component);

				if (open_attempt != m_open_attempt) {
					report.addError(Error(Error::Code::CANCELED, "Request canceled"));
					if (component)
						component->destroy();
					finishLoad(nullptr, rid, report);
					return;
				}

				if (!component) {
					report.addError(Error(Error::Code::NOT_FOUND, "Resource not found", rid));
					finishLoad(nullptr, rid, report);
					return;
				}

				if (!scene) {
					report.addError(Error(Error::Code::INVALID_DATA, "Resource is not a scene", rid));
					component->destroy();
					finishLoad(nullptr, rid, report);
					return;
				}

				if (report.isError()) {
					LOG(LOG_ERROR) << "Failed to load scene: " << rid;
					component->destroy();
					finishLoad(nullptr, rid, report);
					return;
				}

				LOG(LOG_DEBUG) << "Loaded scene: " << rid;

				if (!scene) {
					LOG(LOG_ERROR) << "Loaded component is not a scene: " << rid;
					report.addError(Error(Error::Code::INVALID_DEFINITION, "Invalid scene type"));
					component->destroy();
					finishLoad(nullptr, rid, report);
					return;
				}

				ManagedScene& managed_scene = ensureManagedScene(scene, open_attempt.session, rid, SceneRole::Regular, nullptr);

				// register all scene entry points
				scene->children([this, scene](std::string& name, Component* child) -> bool {
					if (auto* entry = doCast<SceneEntryPoint>(child)) {
						std::string entry_id = entry->getEntryId();
						ManagedScene& managed_scene = m_managed_scenes[scene];
						managed_scene.entry_points[entry_id] = entry;
					}
					return true;
					});

				managed_scene.candidate_generation = open_attempt.generation;
				m_candidate_scenes[open_attempt.generation].push_back(scene);

				finishLoad(scene, rid, report);
				});
			});
	}

	void WorldService::activateScene(const std::string& resource_id, SceneRole role, Scene* owner_scene, OpenHandler handler)
	{
		activateScene(resource_id, m_session_context, role, owner_scene, handler);
	}

	void WorldService::activateScene(const std::string& resource_id, const ResourceContext& context, SceneRole role, Scene* owner_scene, OpenHandler handler)
	{
		if (m_open_attempt.generation != 0) {
			if (handler) {
				handler(Error(Error::Code::UNAVAILABLE, "A world or scene operation is already in progress"));
			}
			return;
		}

		m_open_attempt = {
			++m_open_generation,
			m_session_generation
		};

		doLoadScene(resource_id, m_world, m_open_attempt, context, [this, resource_id, world = m_world, open_attempt = m_open_attempt, role, owner_scene, handler, context](Scene* next_scene, ErrorReport error) {
			if (error.isError()) {
				LOG(LOG_ERROR) << "Activation failed: could not load scene: " << resource_id;
				finishAttempt(open_attempt, error, handler);
				return;
			}
			if (!next_scene) {
				LOG(LOG_ERROR) << "Activation failed: scene not found after loading: " << resource_id;
				error.addError(Error(Error::Code::NOT_FOUND, "Activation failed: scene not found after loading", resource_id));
				finishAttempt(open_attempt, error, handler);
				return;
			}

			// prepare the scene if not already prepared
			std::string rid;
			if (!context.normalizeResourceId(resource_id, rid)) {
				LOG(LOG_ERROR) << "Failed to normalize resource id: " << resource_id;
				error.addError(Error(Error::Code::NOT_FOUND, "Failed to normalize resource id", resource_id));
				finishAttempt(open_attempt, error, handler);
				return;
			}
			ManagedScene& managed_scene = ensureManagedScene(next_scene, open_attempt.session, rid, role, owner_scene);
			m_managed_scenes[next_scene].role = role;
			m_managed_scenes[next_scene].owner_scene = owner_scene;
			if (!managed_scene.isPrepared()) {
				prepareScene(world, next_scene, managed_scene.role, managed_scene.owner_scene, [this, open_attempt, error, handler, next_scene](ErrorReport prepareError) mutable {
					error.merge(std::move(prepareError));
					if (error.isError()) {
						finishAttempt(open_attempt, error, handler);
						return;
					}
					m_managed_scenes[next_scene].state = SceneLifecycleState::Prepared;
					m_managed_scenes[next_scene].lifecycle_managed = true;

					doActivateScene(next_scene, m_managed_scenes[next_scene].role, m_managed_scenes[next_scene].owner_scene);
					finishAttempt(open_attempt, error, handler);
					});
				return;
			}
			doActivateScene(next_scene, m_managed_scenes[next_scene].role, m_managed_scenes[next_scene].owner_scene);
			finishAttempt(open_attempt, error, handler);
			});
	}

	bool WorldService::deactivateScene(const std::string& resource_id)
	{
		return deactivateScene(resource_id, m_session_context);
	}

	bool WorldService::deactivateScene(const std::string& resource_id, const ResourceContext& context)
	{
		// find the loaded scene
		std::string rid;
		if (!context.normalizeResourceId(resource_id, rid)) {
			LOG(LOG_ERROR) << "Failed to normalize resource id: " << resource_id;
			return false;
		}
		Scene* scene_to_deactivate = nullptr;
		for (auto& pair : m_managed_scenes) {
			ManagedScene& managed_scene = pair.second;
			if (managed_scene.resource_id == rid && managed_scene.session_generation == m_session_generation) {
				scene_to_deactivate = managed_scene.scene;
				break;
			}
		}
		if (!scene_to_deactivate) {
			LOG(LOG_WARNING) << "Scene not found for deactivation: " << rid;
			return false;
		}
		return deactivateScene(scene_to_deactivate);
	}
	
	bool WorldService::deactivateScene(Scene* scene_to_deactivate)
	{
		if (!scene_to_deactivate) {
			return false;
		}

		if (scene_to_deactivate == m_primary_scene) {
			LOG(LOG_WARNING) << "Cannot deactivate primary scene: " << scene_to_deactivate->getPathName();
			return false;
		}
		scene_to_deactivate->deactivate();
		scene_to_deactivate->hide();
		return true;
	}

	void WorldService::transitionToScene(const TransitionRequest& request, OpenHandler handler)
	{
		if (m_open_attempt.generation != 0) {
			if (handler) {
				handler(Error(Error::Code::UNAVAILABLE, "A world or scene operation is already in progress"));
			}
			return;
		}

		m_open_attempt = {
			++m_open_generation,
			m_session_generation,
			nullptr,
			false
		};

		// load scene. the loadScene function checks if the scene is already loaded and will call the handler immediately if it is.
		doLoadScene(request.target_resource_id, m_world, m_open_attempt, m_session_context, [this, request, open_attempt = m_open_attempt, handler](Scene* next_scene, ErrorReport error) {
			if (error.isError()) {
				LOG(LOG_ERROR) << "Transition failed: could not load scene: " << request.target_resource_id;
				finishAttempt(open_attempt, error, handler);
				return;
			}

			if (!next_scene) {
				LOG(LOG_ERROR) << "Transition failed: scene not found after loading: " << request.target_resource_id;
				error.addError(Error(Error::Code::NOT_FOUND, "transition scene not found"));
				finishAttempt(open_attempt, error, handler);
				return;
			}

			prepareSceneTransition(m_world, m_primary_scene, next_scene, open_attempt, request, [this, next_scene, request, open_attempt, error = std::move(error), handler](ErrorReport report, SceneTransitionContext context) mutable {
				if (report.isError()) {
					return finishAttempt(open_attempt, report, handler);
				}
				if (open_attempt != m_open_attempt) {
					report.addError(Error(Error::Code::CANCELED, "Request canceled"));
					finishAttempt(open_attempt, report, handler);
					return;
				}
				doActivateScene(next_scene, SceneRole::Regular, nullptr);
				if (report.isError()) {
					return finishAttempt(open_attempt, report, handler);
				}
				Scene* old_scene = m_primary_scene;
				doActivateSceneTransition(next_scene, request, context, open_attempt);
				finishAttempt(open_attempt, report, [this, old_scene, handler](ErrorReport report) {
					if (!report.isError()) {
						PrimarySceneChangedEvent scene_event(this, m_primary_scene, old_scene, m_session_generation);
						sendEvent("primary_scene_changed", &scene_event);
					}
					if (handler) {
						handler(std::move(report));
					}
				});
				});
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

	void WorldService::finishAttempt(OpenAttempt attempt, ErrorReport report, OpenHandler handler)
	{
		const auto [it, first_finish] =	m_finished_attempts.insert(attempt.generation);

		if (!first_finish) {
			return;
		}

		const bool is_current = attempt == m_open_attempt;

		if (!is_current)
			report.addError(Error(Error::Code::CANCELED, "Request canceled"));
		else
			m_open_attempt.finished = true;

		if (report.isError()) {
			for (auto discard_handler : m_discard_handlers) {
				discard_handler(attempt.generation);
			}
			cleanupCandidateRoot(attempt);
		}		

		if (is_current) {
			m_open_attempt.candidate_root = nullptr;
		}

		// success

		if (doCast<World>(m_open_attempt.candidate_root)) {
			m_world = doCast<World>(m_open_attempt.candidate_root);
		}

		if (auto it = m_candidate_scenes.find(attempt.generation);
			it != m_candidate_scenes.end()) {
			for (auto* scene : it->second) {
				auto managed = m_managed_scenes.find(scene);
				if (managed != m_managed_scenes.end() &&
					managed->second.candidate_generation == attempt.generation) {
					managed->second.candidate_generation = 0;
				}
			}
			m_candidate_scenes.erase(it);
		}

		if (is_current)
			m_open_attempt = {};

		if (handler)
			handler(std::move(report));

	}

	void WorldService::onSceneActivate(Scene* scene)
	{
		// only add if not already in the list
		if (scene == nullptr)
			return;

	//	ManagedScene& mgmt_scr = ensureManagedScene(scene, "external", SceneRole::External, nullptr);
	//	mgmt_scr.state = SceneLifecycleState::Active;
	}

	void WorldService::onSceneDeactivate(Scene* scene)
	{
		if (scene == nullptr)
			return;
		if (m_managed_scenes.find(scene) != m_managed_scenes.end()) {
			m_managed_scenes[scene].state = SceneLifecycleState::Prepared;
		}
	}

	void WorldService::onWorldLoaded(OpenAttempt open_attempt, std::string request_id, const ResourceContext& context, ErrorReport report, Component* component, OpenHandler handler)
	{
		// get start scene
		World* world = doCast<World>(component);
		if (world) {
			prepareWorld(world, nullptr, [this, open_attempt, report = std::move(report), handler, context, world](ErrorReport error) mutable {
				report.merge(std::move(error));
				if (report.isError()) {
					finishAttempt(open_attempt, report, handler);
					return;
				}
				startInitialScene(world, nullptr, context, open_attempt, report, handler);
			});
		}
		else if (doCast<Scene>(component)) {
			// if the loaded component is a scene, we can use it directly
			LOG(LOG_INFO) << "Loaded scene: " << request_id;
			Scene* scene = doCast<Scene>(component);
			prepareWorld(nullptr, scene, [this, open_attempt, request_id, report = std::move(report), handler, context, scene](ErrorReport error) mutable {
				report.merge(std::move(error));
				if (report.isError()) {
					finishAttempt(open_attempt, report, handler);
					return;
				}

				ManagedScene& managed_scene = ensureManagedScene(scene, open_attempt.session, request_id, SceneRole::Regular, nullptr);
				managed_scene.lifecycle_managed = true;
				startInitialScene(nullptr, scene, context, open_attempt, report, handler);
				});
		}		
		else
		{
			LOG(LOG_ERROR) << "Loaded scene is not a world or a scene: " << request_id;
			report.addError(Error(Error::Code::INVALID_DEFINITION, "Invalid world type"));
			finishAttempt(open_attempt, report, handler);
		}

	}

	// load a start scene for a world, called after the world is loaded and prepared
	void WorldService::startInitialScene(World* world, Scene* scene, const ResourceContext& context, OpenAttempt open_attempt, ErrorReport report, OpenHandler handler)
	{
		if (!scene) {
			std::string start = world->getValueString("startscene", "");

			if (start.empty()) {
				LOG(LOG_ERROR) << "World has no startscene";
				report.addError(Error(Error::Code::INVALID_DEFINITION, "World has no startscene"));
				finishAttempt(open_attempt, report, handler);
				return;
			}

			// first search within the world.scene.
			Scene* embedded = doCast<Scene>(world->find(start));
			if (embedded) {
				ManagedScene& managed_scene = ensureManagedScene(embedded, open_attempt.session, world->getPath() + "/" + start, SceneRole::Regular, nullptr);
				managed_scene.lifecycle_managed = true;
				startInitialScene(world, embedded, context, open_attempt, report, handler);
				return;
			}

			// Otherwise, load as an external resource ID.
			doLoadScene(start, world, open_attempt, context, [this, world, open_attempt, context, start, report, handler](Scene* next_scene, ErrorReport error) mutable {
				report.merge(std::move(error));
				if (open_attempt != m_open_attempt) {
					report.addError(Error(Error::Code::CANCELED, "Request canceled"));
					finishAttempt(open_attempt, report, handler);
					return;
				}
				if (!next_scene) {
					LOG(LOG_ERROR) << "Start scene not found after loading: " << start;
					report.addError(Error(Error::Code::INVALID_DEFINITION, "Invalid start scene"));
					finishAttempt(open_attempt, report, handler);
					return;
				}
				startInitialScene(world, next_scene, context, open_attempt, report, handler);
				});
			return;
		}

		prepareSceneTransition(world, m_primary_scene, scene, open_attempt, TransitionRequest{},
			[this, world, scene, resource_context = context, open_attempt, report = std::move(report), handler](ErrorReport error, SceneTransitionContext context) mutable {
				report.merge(std::move(error));
				if (report.isError()) {
					return finishAttempt(open_attempt, report, handler);
				}
				if (open_attempt != m_open_attempt) {
					report.addError(Error(Error::Code::CANCELED, "Request canceled"));
					finishAttempt(open_attempt, report, handler);
					return;
				}
				uint64_t old_session_generation = m_session_generation;
				Scene* old_scene = m_primary_scene;
				beginSession(resource_context, open_attempt.session);
				doActivateWorld(world, scene);
				m_world = doCast<World>(open_attempt.candidate_root);
				doActivateScene(scene, SceneRole::Regular, nullptr);
				doActivateSceneTransition(scene, TransitionRequest{}, context, open_attempt);
				m_active_root_resource_id = m_open_attempt.resource_id;
				finishAttempt(open_attempt, report, [this, handler, old_scene, old_session_generation](ErrorReport report) {
					if (!report.isError()) {
						WorldSessionChangedEvent event(this, m_world, old_session_generation, m_session_generation);						
						PrimarySceneChangedEvent scene_event(this, m_primary_scene, old_scene, m_session_generation);
						sendEvent("world_session_changed", &event); 
						sendEvent("primary_scene_changed", &scene_event);
					}
					if (handler) {
						handler(report);
					}
				});
			});
	}

	void WorldService::doActivateSceneTransition(Scene* scene, const TransitionRequest& request, SceneTransitionContext& context, OpenAttempt open_attempt)
	{
		if (m_managed_scenes.find(scene) == m_managed_scenes.end()) {
			LOG(LOG_ERROR) << "Scene is not managed by WorldService: " << scene->getPathName();
			return;
		}
		else if (scene == m_primary_scene) {
			LOG(LOG_WARNING) << "Scene is already the primary scene: " << scene->getPathName();
			return;
		}

		auto old_scene = m_primary_scene;

		// call all scene transition activate handlers
		for (SceneTransitionActivateHandler& activator : m_scene_transition_activate_handlers) {
			activator(&context);
		}

		if (old_scene) {
			// if the old scene is not the primary scene anymore, we can deactivate it
			old_scene->hide();
			old_scene->deactivate();
			m_managed_scenes[old_scene].state = SceneLifecycleState::Prepared;
		}
		m_primary_scene = scene;
	}

	void WorldService::prepareScene(World* world, Scene* next_scene, SceneRole role, Scene* owner_scene, PrepareCompletion completion)
	{
		auto context = std::make_shared<ScenePrepareContext>(ScenePrepareContext{ world, m_primary_scene, next_scene, role, owner_scene });
		ContextRunner<ScenePrepareContext>::Validate validate = [this, open_attempt = m_open_attempt]()->rhea::ErrorReport {
			if (open_attempt != m_open_attempt) {
				LOG(LOG_ERROR) << "Request to prepare world is outdated";
				return Error(Error::Code::CANCELED, "Request to prepare world is outdated");
			}
			return Error();
			};
		ContextRunner<ScenePrepareContext>::run(
			std::move(context),
			m_scene_prepare_handlers, // copy the handlers to avoid issues if the vector is modified during iteration
			std::move(validate),
			std::move([this, completion, next_scene](ErrorReport report) {
				if (!report.isError()) {
					m_managed_scenes[next_scene].lifecycle_managed = true;
					m_managed_scenes[next_scene].state = SceneLifecycleState::Prepared;
				}
				completion(report);
			}));
	}

	void WorldService::prepareWorld(World* world, Scene* standalone_scene, PrepareCompletion completion)
	{
		auto context = std::make_shared<WorldPrepareContext>(WorldPrepareContext{ world, standalone_scene, m_open_attempt.generation });
		ContextRunner<WorldPrepareContext>::Validate validate = [this, open_attempt = m_open_attempt]()->rhea::ErrorReport {
			if (open_attempt != m_open_attempt) {
				LOG(LOG_ERROR) << "Request to prepare world is outdated";
				return Error(Error::Code::CANCELED, "Request to prepare world is outdated");
			}
			return Error();
			};
		ContextRunner<WorldPrepareContext>::run(
			std::move(context),
			m_world_prepare_handlers, // copy the handlers to avoid issues if the vector is modified during iteration
			std::move(validate),
			std::move(completion));
	}

	void WorldService::doActivateWorld(World* world, Scene* standalone_scene)
	{
		WorldPrepareContext context = { world, standalone_scene, m_open_attempt.generation };

		for (WorldActivateHandler& activator : m_world_activate_handlers) {
			activator(&context);
		}
	}

	void WorldService::doActivateScene(Scene* scene, SceneRole role, Scene* owner_scene)
	{
		ScenePrepareContext context = { m_world, m_primary_scene, scene, role, owner_scene };
		for (SceneActivateHandler& activator : m_scene_activate_handlers) {
			activator(&context);
		}
		m_managed_scenes[scene].state = SceneLifecycleState::Active;
		m_managed_scenes[scene].lifecycle_managed = true;

		scene->activate();
		scene->show();
	}

	void WorldService::prepareSceneTransition(
		World* world,
		Scene* source_scene,
		Scene* target_scene,
		OpenAttempt open_attempt,
		const TransitionRequest& request,
		std::function<void(ErrorReport, SceneTransitionContext)> completion)
	{
		ErrorReport report{};
		if (open_attempt != m_open_attempt) {
			report.addError(Error(Error::Code::CANCELED, "Request canceled"));
			completion(report, SceneTransitionContext{});
			return;
		}

		if (m_managed_scenes.find(target_scene) == m_managed_scenes.end()) {
			LOG(LOG_ERROR) << "Scene is not managed by WorldService: " << target_scene->getPathName();
			report.addError(Error(Error::Code::INVALID_DEFINITION, "Invalid scene type"));
			completion(report, SceneTransitionContext{});
			return;
		}
		else if (target_scene == m_primary_scene) {
			LOG(LOG_WARNING) << "Scene is already the primary scene: " << target_scene->getPathName();
			completion(report, SceneTransitionContext{});
			return;
		}
		else if (m_managed_scenes[target_scene].state == SceneLifecycleState::Loaded) {
			// only loaded, not prepared, so we need to prepare it first
			prepareScene(world, target_scene, SceneRole::Regular, nullptr, [this, request, report, completion, world, source_scene, target_scene, open_attempt](ErrorReport error) mutable {
				report.merge(std::move(error));
				if (report.isError()) {
					completion(report, SceneTransitionContext{});
					return;
				}
				// call myself again to continue the transition after preparation
				prepareSceneTransition(world, source_scene, target_scene, open_attempt, request, std::move(completion));
				});
		}
		else {
			// prepare the transition to the new scene
			// get the entry point
			SceneEntryPoint* entry_point = nullptr;
			if (!request.entry_point.empty()) {
				auto it = m_managed_scenes[target_scene].entry_points.find(request.entry_point);
				if (it == m_managed_scenes[target_scene].entry_points.end()) {
					LOG(LOG_ERROR) << "Entry point not found in scene: " << request.entry_point;
					report.addError(Error(Error::Code::INVALID_DEFINITION, "Entry point not found"));
					completion(report, SceneTransitionContext{});
					return;
				}
				entry_point = it->second;
			}
			else if (m_managed_scenes[target_scene].entry_points.find("default") != m_managed_scenes[target_scene].entry_points.end()) {
				// use the default entry point if no specific entry point is requested
				entry_point = m_managed_scenes[target_scene].entry_points["default"];
			}

			bool starts_new_session = open_attempt.session != m_session_generation;
			auto context = std::make_shared<SceneTransitionContext>(SceneTransitionContext{ world, source_scene, target_scene, m_open_attempt.generation, starts_new_session, entry_point, request});
			ContextRunner<SceneTransitionContext>::Validate validate = [this, open_attempt = m_open_attempt]()->rhea::ErrorReport {
				if (open_attempt != m_open_attempt) {
					LOG(LOG_ERROR) << "Request to prepare world is outdated";
					return Error(Error::Code::CANCELED, "Request to prepare world is outdated");
				}
				return Error();
				};
			ContextRunner<SceneTransitionContext>::run(
				context,
				m_scene_transition_handlers, // copy the handlers to avoid issues if the vector is modified during iteration
				std::move(validate),
				[completion,context](ErrorReport report){
					completion(report, *context);
				});
		}
	}

	WorldService::ManagedScene& WorldService::ensureManagedScene(Scene* scene, uint64_t session_generation, std::string resourceId, SceneRole role, Scene* owner)
	{
		auto it = m_managed_scenes.find(scene);
		if (it != m_managed_scenes.end() && it->second.session_generation == session_generation) {
			return it->second;
		}
		else {
			ManagedScene managed_scene;
			managed_scene.scene = scene;
			managed_scene.resource_id = resourceId;
			managed_scene.role = role;
			managed_scene.owner_scene = owner;
			managed_scene.state = SceneLifecycleState::Loaded;
			managed_scene.session_generation = session_generation;
			m_managed_scenes[scene] = managed_scene;
			return m_managed_scenes[scene];
		}
	}

	void WorldService::cleanupCandidateRoot(OpenAttempt attempt)
	{
		// 1. gather all objects to be deleted from the attempt
		std::unordered_set<Unit*> candidates;
		if (attempt.candidate_root)
			candidates.insert(attempt.candidate_root);

		if (auto it = m_candidate_scenes.find(attempt.generation);
			it != m_candidate_scenes.end()) {
			for (auto* scene : it->second) {
				if (scene)
					candidates.insert(scene);
			}
			m_candidate_scenes.erase(it);
		}

		if (attempt == m_open_attempt)
			m_open_attempt.candidate_root = nullptr;

		// 2. Keep only top-level roots. Children will be destroyed with their parents.
		std::vector<Unit*> roots;
		for (auto* candidate : candidates) {
			auto* parent = candidate->getParent();
			while (parent && candidates.count(parent) == 0)
				parent = parent->getParent();

			if (!parent)
				roots.push_back(candidate);
		}

		// 3. Gather scenes while the tree is still unchanged.
		std::vector<Scene*> scenes;
		std::unordered_set<Scene*> doomed_scenes;

		for (auto* root : roots) {
			root->traverse([&](std::string&, Component* component) {
				if (auto* scene = doCast<Scene>(component)) {
					scenes.push_back(scene);
					doomed_scenes.insert(scene);
				}
				return true;
				});
		}

		// 4. Unregister all scenes while their objects are still alive.
		for (auto it = scenes.rbegin(); it != scenes.rend(); ++it) {
			auto* scene = *it;
			if (m_primary_scene == scene)
				m_primary_scene = nullptr; // deactivateScene protects the Primary Scene.

			deactivateScene(scene);
			m_managed_scenes.erase(scene);
		}

		for (auto& [scene, managed] : m_managed_scenes) {
			if (doomed_scenes.count(managed.owner_scene))
				managed.owner_scene = nullptr;
		}

		// 5. Now destroy() can recursively release the subtrees.
		if (m_world && candidates.count(m_world))
			m_world = nullptr;

		for (auto* root : roots)
			root->destroy();
	}
}
