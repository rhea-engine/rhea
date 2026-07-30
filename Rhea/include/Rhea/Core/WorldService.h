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

		enum class OpenError
		{
			None,
			ResourceNotFound,
			InvalidWorldType,
			InvalidStartScene,
			InvalidSceneType,
			StartSceneLoadFailed,
			EntryPointNotFound,
			RequestCanceled
		};

		enum class PrepareError
		{
			None,
			PhysicsInitializationFailed,
			MissingCamera,
			InvalidController
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

		enum class TransitionError
		{
			None,
			AlreadyTransitioning,
			ResourceNotFound,
			InvalidSceneType,
			PreparationFailed,
			EntryPointNotFound,
			ActivationFailed,
			RequestCanceled
		};

		struct WorldPrepareContext
		{
			World* world = nullptr;
			Scene* standaloneScene = nullptr;
		};

		struct ScenePrepareContext
		{
			World* world = nullptr;
			Scene* current_primary_scene = nullptr;
			Scene* scene = nullptr;

			WorldService::SceneRole role = WorldService::SceneRole::External;
			Scene* owner_scene = nullptr;
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

			SceneEntryPoint* entry_point = nullptr;
			const TransitionRequest* request = nullptr;
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
			WorldSessionChangedEvent(WorldService* service,	World* world, uint64_t previous, uint64_t current)
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

		using OpenHandler =	std::function<void(OpenError)>;
		using LoadSceneHandler = std::function<void(Scene*, OpenError)>;
		using TransitionCompletion = std::function<void(TransitionError)>;

		using WorldPrepareHandler = std::function<PrepareError(WorldPrepareContext*)>;
		using ScenePrepareHandler = std::function<PrepareError(ScenePrepareContext*)>;
		using TransitionHandler = std::function<void(SceneTransitionContext*, TransitionCompletion)>;


		void openWorld(std::string& resource_id, OpenHandler handler = nullptr);
		// scene lifecycle management
		// load a scene without transitioning to it
		void loadScene(std::string resource_id, LoadSceneHandler handler = nullptr);
		// activate a scene without transitioning to it
		void activateScene(std::string& resource_id, SceneRole role = SceneRole::Regular, Scene* owner_scene = nullptr, OpenHandler handler = nullptr);
		// deactivate a scene
		// return false if scene was primary scene and cannot be deactivated
		bool deactivateScene(std::string& resource_id);
		// transition to a scene
		void transitionToScene(const TransitionRequest request, OpenHandler handler = nullptr);

		// get current state
		World* getWorld() const { return m_world; }
		Scene* getPrimaryScene() const { return m_primary_scene; }
		State getState() const { return m_state; }
		bool isPrimaryScene(const Scene* scene) const {	return scene == m_primary_scene; }

		// add preparation handlers
		// prepare handlers are called in the order they are added, and if any of them returns an error, the preparation is aborted

		// world prepare handlers are called once when a world is loaded, before the start scene is loaded
		void addWorldPrepareHandler(WorldPrepareHandler handler) { m_world_prepare_handlers.push_back(handler); }

		// scene prepare handlers are called once when a scene is loaded, before it is activated
		void addScenePrepareHandler(ScenePrepareHandler handler) { m_scene_prepare_handlers.push_back(handler); }

		// handler for prepare a scene for activation, called before a scene activated		
		// add transition handler for scene transition, called before a scene is activated, old and new are intedned as primary scene, 
		// and the transition is aborted if any of the handlers returns an error
		void addSceneTransitionHandler(TransitionHandler handler) { m_scene_transition_handlers.push_back(handler); }

		// every frame
		bool onFrame(float delta) override;
		bool onFrameFinal(float delta) override;

	protected:
		// handle activate and deactivate scene events
		void onSceneActivate(Scene* scene);
		void onSceneDeactivate(Scene* scene);

		void beginSession();
		void deactivatePreviousSession();
		void onWorldLoaded(uint64_t open_generation, std::string request_id, Component* component, OpenHandler handler);

		void loadStartScene(World* world, uint64_t open_generation, OpenHandler handler);
		// activate loaded scene and make it primary scene
		void doTransitionScene(Scene* scene, const TransitionRequest* request, OpenHandler handler);

		PrepareError prepareScene(Scene* next_scene, SceneRole role, Scene* owner_scene = nullptr);
		PrepareError prepareWorld(World* world, Scene* standalone_scene = nullptr);
		void prepareSceneTransition(Scene* source_scene, Scene* target_scene, SceneEntryPoint* entry_point, const TransitionRequest* request, TransitionCompletion completion);
		void prepareSceneTransitionAsync(std::shared_ptr<SceneTransitionContext> context, size_t handlerIndex, TransitionCompletion complete);

		ManagedScene& ensureManagedScene(Scene* scene, std::string resourceId, SceneRole role, Scene* owner = nullptr);

		World* m_world = nullptr;

		// list of active scenes	
		std::unordered_map<Scene*, ManagedScene> m_managed_scenes;
		Scene* m_primary_scene = nullptr;

		State m_state = State::Idle;

		// prevent stale results from async world load requests
		uint64_t m_open_generation = 0;
		uint64_t m_session_generation = 0;

		// pending scene loads
		struct SceneLoadKey
		{
			uint64_t session_generation;
			std::string resource_id;

			bool operator==(const SceneLoadKey& other) const
			{
				return session_generation == other.session_generation &&
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
				return std::hash<uint64_t>()(key.session_generation) ^ std::hash<std::string>()(key.resource_id);
			}
		};

		std::unordered_map<SceneLoadKey, PendingSceneLoad, SceneLoadKeyHash> m_pending_scene_loads;

		std::vector<ScenePrepareHandler> m_scene_prepare_handlers;
		std::vector<WorldPrepareHandler> m_world_prepare_handlers;
		std::vector<TransitionHandler> m_scene_transition_handlers;
	};
}
#endif // !RHEA_WORLDSERVICE_H
