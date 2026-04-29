/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#ifndef RHEA_RHEAPHYSICSFABRIC_H
#define RHEA_RHEAPHYSICSFABRIC_H

#include <Rhea/Application.h>
#include <Rhea/FabricSystem.h>

namespace rhea::physics
{
	/** Fabrics for RheaPhysics Null Backend
	 * -------------------------------------
	 * 
	 * The `RheaPhysicsFabric` class serves as the integration point for the RheaPhysics module within the Rhea application framework. It implements the `IFabric` interface, allowing it to register the physics services and components provided by RheaPhysics when the application initializes.
	 *
	 * Responsibilities:
	 * - Registering null physics-related services  with the application.
	 * - Ensuring that all necessary components and systems for null physics are properly initialized and available for use in the application.
	 *
	 * Usage:
	 * - The `RheaPhysicsFabric` is typically instantiated and registered with the application during the setup phase, allowing the physics services to be seamlessly integrated into the application's lifecycle.
	*/
	class RheaPhysicsFabric : public rhea::IFabric
	{
	public:
		void Register(rhea::Application* app) override;

		const char* getName() const override { return "RheaPhysics"; }
	};
}

#endif // !RHEA_RHEAPHYSICSFABRIC_H