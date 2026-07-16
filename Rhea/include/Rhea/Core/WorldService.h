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

	struct WorldContext
	{
		World* world;
		Scene* next_scene;
		Scene* current_primary_scene;
	};

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
		};

		WorldService(std::string name, std::string type_name) :
			Service(name, type_name)
		{}

		// init event
		virtual void onInit();

		using OpenHandler =	std::function<void(OpenError)>;
		using PrepareHandler = std::function<PrepareError(WorldContext* context)>;

		void openWorld(std::string& resource_id, OpenHandler handler = nullptr);
		// scene lifecycle management
		// load a scene without transitioning to it
		void loadScene(std::string& resource_id, OpenHandler handler = nullptr);
		// transition to a scene
		void transitionToScene(std::string& resource_id, OpenHandler handler = nullptr);

		// get current state
		World* getWorld() const { return m_world; }
		Scene* getPrimaryScene() const { return m_primary_scene; }
		State getState() const { return m_state; }

		// add preparation handlers
		void addPrepareHandler(PrepareHandler handler) { m_prepare_handlers.push_back(handler); }
		void addWorldPrepareHandler(PrepareHandler handler) { m_world_prepare_handlers.push_back(handler); }

		// every frame
		bool onFrame(float delta) override;
		bool onFrameFinal(float delta) override;

	protected:
		// prepare a scene to display
		void onSceneActivate(Scene* scene);
		void onSceneDeactivate(Scene* scene);

		void onWorldLoaded(uint64_t requestId, Component* component, OpenHandler handler);

		void loadStartScene(uint64_t requestId, OpenHandler handler);
		// activate loaded scene and make it primary scene
		void onSceneLoaded(Scene* scene, OpenHandler handler);

		PrepareError prepareScene(Scene* next_scene);
		PrepareError prepareWorld(World* world);

		World* m_world = nullptr;

		// list of active scenes	
		std::unordered_map<Scene*, ManagedScene> m_managed_scenes;
		Scene* m_primary_scene = nullptr;
		Scene* m_next_scene = nullptr;

		State m_state = State::Idle;

		uint64_t m_request_id = 0;
		std::string m_request_path;

		std::vector<PrepareHandler> m_prepare_handlers;
		std::vector<PrepareHandler> m_world_prepare_handlers;
	};
}
#endif // !RHEA_WORLDSERVICE_H
