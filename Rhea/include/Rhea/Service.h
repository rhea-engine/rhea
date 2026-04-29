/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#ifndef RHEA_SERVICE_H
#define RHEA_SERVICE_H

#include <string>

#include <Rhea/Unit.h>

namespace rhea
{
	class Application;

	class Service : public Unit
	{
		MECLASS(Service, Unit)
	public:
		Service(std::string service_name, std::string type_name) :
			Unit(service_name, type_name)
		{}
		Service(std::string service_name) :
			Service(service_name, "//meworla/service")
		{}

		//  events
		
		// init event
		virtual void onInit() {};

		// every frame

		/**
		 * onPrePhysics
		 * -------------
		 * Called for each service before the physics simulation step.
		 * - Allows services to compute inputs for the physics system.
		 * - Uses `elapsed_s` to scale calculations (frame-based timing).
		 * Examples:
		 * - Moving AI-controlled characters.
		 * - Computing forces or inputs for physics bodies.
		 */
		virtual bool onPrePhysics(float delta) { return false; };

		/**
		 * onPhysicsStep
		 * ----
		 * Called during each physics simulation step at a fixed timestep.
		 *
		 * Parameters:
		 * - `frame_slice`: The fixed timestep (in seconds) for this physics update.
		 *   - This value is consistent across all steps and ensures physics stability and determinism.
		 *
		 * Responsibilities:
		 * - Implement behavior that needs to run in sync with the physics engine.
		 * - Update internal states based on the fixed timestep, such as:
		 *   - Applying forces, impulses, or movement.
		 *   - Resolving game logic tied to physics, such as collision responses.
		 * - Services that do not need physics updates should provide an empty implementation.
		 *
		 * Examples of Use:
		 * - The physics service: Advancing the physics simulation and resolving collisions.
		 * - The world service: Updating positions of objects or handling game logic tied to physics.
		 *
		 * Notes:
		 * - The `step` function is called repeatedly in the game loop for as many fixed-timestep updates
		 *   as needed to "catch up" with real-time.
		 * - Avoid heavy computations or gameplay logic that doesn't rely on fixed-timestep updates.
		 */
		virtual void onPhysicsStep(float frame_slice) {};
		
		/**
		 * onFrame
		 * -------
		 * Called for each service after the physics simulation step.
		 * - Updates systems based on the current frame's state.
		 * - Uses `elapsed_s` for frame-based calculations.
		 * Examples:
		 * - Animating sprites or models.
		 * - Updating AI behavior.
		 */
		virtual bool onFrame(float delta) { return false; };

		/**
		 * onFrameFinal
		 * -------------
		 * Called for each service at the end of the frame.
		 * - Performs final updates before rendering.
		 * - Typically used for cleanup or preparing data for rendering.
		 */
		virtual bool onFrameFinal(float delta) { return false; };

	protected:
		
	};
}

#endif // !RHEA_SERVICE_H
