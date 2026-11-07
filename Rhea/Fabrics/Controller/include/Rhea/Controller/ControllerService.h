/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */
#ifndef RHEA_CONTROLLERSERVICE_H
#define RHEA_CONTROLLERSERVICE_H

#include <Rhea/Service.h>
#include <Rhea/Core/Controller.h>
#include <Rhea/Core/WorldService.h>
#include <cstdint>
#include <functional>
#include <vector>

namespace rhea
{
	using PlayerSlotHandle = int;
	constexpr PlayerSlotHandle InvalidPlayerSlot = -1;

	class Camera;

	class ControllerService : public Service
	{
		RECLASS(ControllerService, Service, "//meworla/controllerservice")
	public:
		struct ControllerBinding
		{
			Controller* controller = nullptr;
			PlayerSlotHandle player_slot = InvalidPlayerSlot;
			int controller_id = -1;
		};

		struct PlacementContext
		{
			WorldService::SceneTransitionContext* transition = nullptr;

			PlayerSlotHandle player_slot = InvalidPlayerSlot;

			Controller* controller = nullptr;
			Item* source_character = nullptr;
			Item* target_character = nullptr;
		};

		enum class PlacementResult
		{
			Unhandled,
			Placed,
			Failed,
			KeepInput,
			PlacedKeepInput
		};
		using PlacementHandler = std::function<PlacementResult(PlacementContext&)>;

		ControllerService(std::string name, std::string type_name);

		void onInit() override;

		bool onFrame(float delta) override;

		Controller* getPrimaryController() const;
		Controller* getController(PlayerSlotHandle player_slot) const;
		template<class T>
		T* getController(PlayerSlotHandle player_slot) const
		{
			return doCast<T>(getController(player_slot));
		}

		void addPlacementHandler(PlacementHandler handler, int priority = 0);

	private:
		// we assume only one world is active at a time, so we can keep a list of controllers for the current world
		std::vector<ControllerBinding> m_controllers;		
		struct PlacementSlot
		{
			PlacementHandler handler;
			int priority = 0;
		};
		std::vector<PlacementSlot> m_placement_handlers;

		// transition handlers
		void onWorldPrepare(WorldService::WorldPrepareContext* context, WorldService::PrepareCompletion completion);
		void onWorldActivate(WorldService::WorldPrepareContext* context);
		void onSceneTransitionPrepare(WorldService::SceneTransitionContext* context, WorldService::PrepareCompletion completion);
		void onSceneTransitionActivate(WorldService::SceneTransitionContext* context);
		void onDiscard(uint64_t generation);

		// only one world may be loaded at a time, so we can keep track of the prepared controllers for the current world
		struct PreparedControllers {
			uint64_t generation = 0;
			Scene* root = nullptr;
			int next_controller_id = 0;
			std::vector<ControllerBinding> bindings;
		};

		PreparedControllers m_prepared_controllers;

		struct PreparedBinding {
			Controller* controller = nullptr;
			Item* character = nullptr;
			bool character_is_candidate = false;
			Camera* camera = nullptr;
			bool camera_is_candidate = false;
			int controller_id = -1;
			bool reset_input = true;
		};

		struct PreparedTransition {
			uint64_t generation = 0;
			std::vector<PreparedBinding> bindings;
		};

		PreparedTransition m_prepared_transition;
	};
}


#endif // RHEA_CONTROLLERSERVICE_H