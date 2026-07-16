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

		WorldService(std::string name, std::string type_name) :
			Service(name, type_name)
		{}

		// init event
		virtual void onInit();

		using OpenHandler =	std::function<void(OpenError)>;
		using PrepareHandler = std::function<PrepareError(WorldContext* context)>;

		// scene lifecycle management
		void openWorld(std::string& resource_id, OpenHandler handler = nullptr);

		World* getWorld() const { return m_world; }
		Scene* getPrimaryScene() const { return m_primary_scene; }
		State getState() const { return m_state; }

		void addPrepareHandler(PrepareHandler handler) { m_prepare_handlers.push_back(handler); }
		void addWorldPrepareHandler(PrepareHandler handler) { m_world_prepare_handlers.push_back(handler); }

		// every frame
		bool onFrame(float delta) override;
		bool onFrameFinal(float delta) override;

	protected:
		// prepare a scene to display
		void onSceneActivate(Scene* scene);
		void onSceneDeactivate(Scene* scene);

		// list of active scenes
		std::vector<Scene*> m_scenes;

		void onWorldLoaded(uint64_t requestId, Component* component, OpenHandler handler);

		void loadStartScene(uint64_t requestId, OpenHandler handler);
		void onSceneLoaded(Scene* scene, OpenHandler handler);

		PrepareError prepareScene(Scene* next_scene);
		PrepareError prepareWorld(World* world);

		World* m_world = nullptr;
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
