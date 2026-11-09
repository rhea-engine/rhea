/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#ifndef RHEA_WORLDSERVICE_H
#define RHEA_WORLDSERVICE_H

#include <Rhea/Service.h>
#include "Rhea/Core/Scene.h"
#include <Rhea/Core/World.h>
#include <Rhea/Application.h>
#include <Rhea/Event.h>
#include <Rhea/Core/SceneEntryPoint.h>
#include <Rhea/Resources/ResourceContext.h>
#include <unordered_map>
#include <unordered_set>
#include <string>
#include <functional>

namespace rhea
{
	/** The main service that manages scene and their lifecycles
	*/

	class WorldService : public Service
	{
		MECLASS(WorldService, Service)
	public:

		enum class State
		{
			Idle,
			LoadingWorld,
			Ready,
			Transitioning
		};

		enum class SceneRole
		{
			Regular,
			Persistent,
			Partition,
			Overlay,
			External
		};

		enum class SceneLifecycleState
		{
			Loaded,
			Prepared,
			Active
		};
		
		struct WorldPrepareContext
		{
			World* world = nullptr;
			Scene* standalone_scene = nullptr;
			uint64_t open_generation = 0;
		};

		struct ScenePrepareContext
		{
			World* world = nullptr;
			Scene* current_primary_scene = nullptr;
			Scene* scene = nullptr;

			WorldService::SceneRole role = WorldService::SceneRole::External;
			Scene* owner_scene = nullptr;
			uint64_t open_generation = 0;
		};

		struct TransitionRequest
		{
			std::string target_resource_id;
			std::string entry_point;

			std::unordered_map<std::string, Variant> parameters;

			bool unload_previous = false;
		};

		struct SceneTransitionContext
		{
			World* world = nullptr;

			Scene* source_scene = nullptr;
			Scene* target_scene = nullptr;
			uint64_t open_generation = 0;
			bool starts_new_session = false;

			SceneEntryPoint* entry_point = nullptr;
			TransitionRequest request;
		};

		struct ManagedScene
		{
			Scene* scene = nullptr;
			SceneRole role = SceneRole::External;
			SceneLifecycleState state = SceneLifecycleState::Loaded;

			// logical relation, no unit-parent relation
			Scene* owner_scene = nullptr;

			std::string resource_id;
			uint64_t session_generation = 0;
			uint64_t candidate_generation = 0; // if the scene is a candidate for a transition, this is the generation of the transition request that made it a candidate

			// lifecycle management
			bool lifecycle_managed = false;

			std::unordered_map<std::string, SceneEntryPoint*> entry_points;

			bool isPrepared() const { return state == SceneLifecycleState::Prepared || state == SceneLifecycleState::Active; }
		};

		class PrimarySceneChangedEvent : public Event
		{
		public:
			PrimarySceneChangedEvent(WorldService* service, Scene* current_scene, Scene* previous_scene, uint64_t session_generation) :
				Event("primary_scene_changed", nullptr),
				service(service),
				current_scene(current_scene),
				previous_scene(previous_scene),
				session_generation(session_generation)
			{}
			WorldService* service = nullptr;
			Scene* current_scene = nullptr;
			Scene* previous_scene = nullptr;
			uint64_t session_generation = 0;
		};

		class WorldSessionChangedEvent : public Event
		{
		public:
			WorldSessionChangedEvent(WorldService* service, World* world, uint64_t previous, uint64_t current)
				: Event("world_session_changed", nullptr),
				service(service),
				world(world),
				previous_generation(previous),
				current_generation(current)
			{}

			WorldService* service;
			World* world;
			uint64_t previous_generation;
			uint64_t current_generation;
		};

		WorldService(std::string name, std::string type_name) :
			Service(name, type_name)
		{}

		// init event
		virtual void onInit();

		using OpenHandler = std::function<void(ErrorReport)>;
		using LoadSceneHandler = std::function<void(Scene*, ErrorReport)>;
		using PrepareCompletion = std::function<void(ErrorReport)>;

		// asynchronous prepare handlers for world and scene, called before a world or scene is activated, 
		// and if any of them returns an error, the activation is aborted
		using WorldPrepareHandler = std::function<void(WorldPrepareContext*, PrepareCompletion)>;
		using ScenePrepareHandler = std::function<void(ScenePrepareContext*, PrepareCompletion)>;
		using TransitionPrepareHandler = std::function<void(SceneTransitionContext*, PrepareCompletion)>;
		using DiscardHandler = std::function<void(uint64_t)>;

		// activation handlers for world and scene activation, synchronous, called while a world or scene is being activated
		using WorldActivateHandler = std::function<void(WorldPrepareContext*)>;
		using SceneActivateHandler = std::function<void(ScenePrepareContext*)>;
		using SceneTransitionActivateHandler = std::function<void(SceneTransitionContext*)>;

		void openWorld(const std::string& resource_id, OpenHandler handler = nullptr);
		
		// scene lifecycle management
		// load a scene without transitioning to it
		void loadScene(const std::string& resource_id, LoadSceneHandler handler = nullptr);
		void loadScene(const std::string& resource_id, const ResourceContext& context, LoadSceneHandler handler = nullptr);
		// activate a scene without transitioning to it
		void activateScene(const std::string& resource_id, SceneRole role = SceneRole::Regular, Scene* owner_scene = nullptr, OpenHandler handler = nullptr);
		void activateScene(const std::string& resource_id, const ResourceContext& context, SceneRole role = SceneRole::Regular, Scene* owner_scene = nullptr, OpenHandler handler = nullptr);
		// deactivate a scene
		// return false if scene was primary scene and cannot be deactivated
		bool deactivateScene(const std::string& resource_id);
		bool deactivateScene(const std::string& resource_id, const ResourceContext& context);
		bool deactivateScene(Scene* scene_to_deactivate);
		// transition to a scene
		void transitionToScene(const TransitionRequest& request, OpenHandler handler = nullptr);

		// get current state
		World* getWorld() const { return m_world; }
		Scene* getPrimaryScene() const { return m_primary_scene; }
		bool isPrimaryScene(const Scene* scene) const { return scene == m_primary_scene; }

		// add preparation handlers
		// prepare handlers are called in the order they are added, and if any of them returns an error, the preparation is aborted

		// world prepare handlers are called once when a world is loaded, before the start scene is loaded
		void addWorldPrepareHandler(WorldPrepareHandler handler) { m_world_prepare_handlers.push_back(handler); }

		// scene prepare handlers are called once when a scene is loaded, before it is activated
		void addScenePrepareHandler(ScenePrepareHandler handler) { m_scene_prepare_handlers.push_back(handler); }

		// handler for preparing a scene for activation, called before a scene activated		
		// add transition handler for scene transition, called before a scene is activated, old and new are intended as primary scene, 
		// and the transition is aborted if any of the handlers returns an error
		void addSceneTransitionPrepareHandler(TransitionPrepareHandler handler) { m_scene_transition_handlers.push_back(handler); }

		// world and scene activation handlers are called when a world or scene is activated, and if any of them returns an error, the activation is aborted
		void addWorldActivateHandler(WorldActivateHandler handler) { m_world_activate_handlers.push_back(handler); }
		void addSceneActivateHandler(SceneActivateHandler handler) { m_scene_activate_handlers.push_back(handler); }
		void addSceneTransitionActivateHandler(SceneTransitionActivateHandler handler) { m_scene_transition_activate_handlers.push_back(handler); }
		// discard handlers
		void addDiscardHandler(DiscardHandler handler) { m_discard_handlers.push_back(handler); }

		// every frame
		bool onFrame(float delta) override;
		bool onFrameFinal(float delta) override;

	protected:
		struct OpenAttempt {
			uint64_t generation = 0;
			uint64_t session = 0;
			World* candidate_world = nullptr;
			Unit* candidate_root = nullptr;
			bool finished = false;
			std::string resource_id;

			bool operator ==(const OpenAttempt& other) const {
				return generation == other.generation && session == other.session;
			}
			bool operator != (const OpenAttempt& other) const {
				return !(*this == other);
			}
		};
		void finishAttempt(OpenAttempt attempt, ErrorReport report, OpenHandler handler);
		void cleanupCandidateRoot(OpenAttempt attempt);
		void destroySceneRoots(const std::unordered_set<Unit*>& candidates);

		// handle activate and deactivate scene events
		void onSceneActivate(Scene* scene);
		void onSceneDeactivate(Scene* scene);

		void beginSession(const ResourceContext& context, uint64_t next_session_generation);
		void deactivateSession(uint64_t session_generation);
		void releaseSession(uint64_t session_generation, Unit* session_root);
		void loadWorld(const std::string& rid, const std::string& realm_alias, OpenAttempt open_attempt, OpenHandler handler, bool allow_collection = true);
		void doLoadScene(const std::string& resource_id, World* world, OpenAttempt open_attempt, const ResourceContext& context, LoadSceneHandler handler);

		// internal for open world and scene
		void onWorldLoaded(OpenAttempt open_attempt, const std::string request_id, const ResourceContext& context, ErrorReport report, Component* component, OpenHandler handler);
		void startInitialScene(World* world, Scene* scene, const ResourceContext& context, OpenAttempt open_attempt, ErrorReport report, OpenHandler handler);
		
		// prepare handlers for world and scene, called before a world or scene is activated, and if any of them returns an error, the activation is aborted
		void prepareScene(World* world, Scene* next_scene, SceneRole role, Scene* owner_scene, PrepareCompletion complete);
		void prepareWorld(World* world, Scene* standalone_scene, PrepareCompletion complete);
		void prepareSceneTransition(World* world, Scene* source_scene, Scene* target_scene, OpenAttempt open_attempt, const TransitionRequest& request,
			std::function<void(ErrorReport, SceneTransitionContext)> completion);
		
		// synchronous activation handlers for world and scene activation, called while a world or scene is being activated, and if any of them returns an error, the activation is aborted
		void doActivateWorld(World* world, Scene* standalone_scene);
		void doActivateScene(Scene* scene, SceneRole role, Scene* owner_scene = nullptr);
		void doActivateSceneTransition(Scene* scene, const TransitionRequest& request, SceneTransitionContext& context, OpenAttempt open_attempt);

		ManagedScene& ensureManagedScene(Scene* scene, uint64_t session_generation, std::string resourceId, SceneRole role, Scene* owner = nullptr);

		struct PreparedWorld {
			std::string load_rid;
			std::string public_realm;   // e.g. "world1"; empty for unchanged addresses
			std::string project_realm;  // e.g. "project-realm-8"
		};
		using PrepareWorldHandler = std::function<void(PreparedWorld, ErrorReport)>;
		void prepareWorldAddress(const std::string& rid, uint64_t& lookup_generation, PrepareWorldHandler handler);

		World* m_world = nullptr;

		// list of active scenes	
		std::unordered_map<Scene*, ManagedScene> m_managed_scenes;
		Scene* m_primary_scene = nullptr;
		std::string m_active_root_resource_id;

		// prevent stale results from async world load requests
		OpenAttempt m_open_attempt;
		std::unordered_map<uint64_t, std::vector<Scene*>> m_candidate_scenes;
		std::unordered_set<uint64_t> m_finished_attempts;

		uint64_t m_open_generation = 0;
		// curent active session
		uint64_t m_session_generation = 0;
		// the next session id to use for a new openWorld
		uint64_t m_next_session_generation = 0;
		ResourceContext m_session_context{};

		// session to be relased after the next scene transition is complete

		// pending scene loads
		struct SceneLoadKey
		{
			uint64_t open_generation;
			uint64_t session_generation;
			std::string resource_id;

			bool operator==(const SceneLoadKey& other) const
			{
				return session_generation == other.session_generation && open_generation == other.open_generation &&
					resource_id == other.resource_id;
			}
		};

		struct PendingSceneLoad
		{
			std::vector<LoadSceneHandler> handlers;
		};

		struct SceneLoadKeyHash
		{
			std::size_t operator()(const SceneLoadKey& key) const
			{
				return std::hash<uint64_t>()(key.open_generation) ^ 
					std::hash<uint64_t>()(key.session_generation) ^ 
					std::hash<std::string>()(key.resource_id);
			}
		};

		std::unordered_map<SceneLoadKey, PendingSceneLoad, SceneLoadKeyHash> m_pending_scene_loads;

		std::vector<ScenePrepareHandler> m_scene_prepare_handlers;
		std::vector<WorldPrepareHandler> m_world_prepare_handlers;
		std::vector<TransitionPrepareHandler> m_scene_transition_handlers;

		std::vector<WorldActivateHandler> m_world_activate_handlers;
		std::vector<SceneActivateHandler> m_scene_activate_handlers;
		std::vector<SceneTransitionActivateHandler> m_scene_transition_activate_handlers;
		std::vector<DiscardHandler> m_discard_handlers;
	};
}
#endif // !RHEA_WORLDSERVICE_H
