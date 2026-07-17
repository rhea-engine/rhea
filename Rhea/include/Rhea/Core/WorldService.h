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
			LoadingStartScene,
			Ready,
			Transitioning,
			Failed
		};

		enum class OpenError
		{
			None,
			ResourceNotFound,
			InvalidWorldType,
			InvalidStartScene,
			StartSceneLoadFailed,
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
			Primary,
			Persistent,
			Partition,
			Overlay,
			External
		};

		enum class SceneLifecycleState
		{
			Loaded,
			Prepared,
			Active,
			Shown
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

			Unit* entry_point = nullptr;
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

			// loaded by world service?
			bool owned_by_worldservice = false;

			bool isPrimaryScene() const { return role == SceneRole::Primary; }
			bool isPrepared() const { return state == SceneLifecycleState::Prepared || state == SceneLifecycleState::Active || state == SceneLifecycleState::Shown; }
		};

		WorldService(std::string name, std::string type_name) :
			Service(name, type_name)
		{}

		// init event
		virtual void onInit();

		using OpenHandler =	std::function<void(OpenError)>;

		using WorldPrepareHandler = std::function<PrepareError(WorldPrepareContext* context)>;
		using ScenePrepareHandler = std::function<PrepareError(ScenePrepareContext* context)>;
		using TransitionHandler = std::function<TransitionError(SceneTransitionContext* context)>;


		void openWorld(std::string& resource_id, OpenHandler handler = nullptr);
		// scene lifecycle management
		// load a scene without transitioning to it
		void loadScene(std::string resource_id, OpenHandler handler = nullptr);
		// activate a scene without transitioning to it
		void activateScene(std::string& resource_id, OpenHandler handler = nullptr);
		// deactivate a scene
		// return false if scene was primary scene and cannot be deactivated
		bool deactivateScene(std::string& resource_id);
		// transition to a scene
		void transitionToScene(const TransitionRequest* request, OpenHandler handler = nullptr);

		// get current state
		World* getWorld() const { return m_world; }
		Scene* getPrimaryScene() const { return m_primary_scene; }
		State getState() const { return m_state; }

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

		void onWorldLoaded(uint64_t requestId, Component* component, OpenHandler handler);

		void loadStartScene(uint64_t requestId, OpenHandler handler);
		// activate loaded scene and make it primary scene
		void doTransitionScene(Scene* scene, TransitionRequest* request, OpenHandler handler);

		PrepareError prepareScene(Scene* next_scene, SceneRole role, Scene* owner_scene = nullptr);
		PrepareError prepareWorld(World* world);
		PrepareError prepareSceneTransition(Scene* source_scene, Scene* target_scene, Unit* entry_point, const TransitionRequest* request);

		World* m_world = nullptr;

		// list of active scenes	
		std::unordered_map<Scene*, ManagedScene> m_managed_scenes;
		Scene* m_primary_scene = nullptr;
		Scene* m_next_scene = nullptr;

		State m_state = State::Idle;

		uint64_t m_request_id = 0;
		std::string m_request_path;

		std::vector<ScenePrepareHandler> m_scene_prepare_handlers;
		std::vector<WorldPrepareHandler> m_world_prepare_handlers;
		std::vector<TransitionHandler> m_scene_transition_handlers;
	};
}
#endif // !RHEA_WORLDSERVICE_H
