#ifndef RHEA_RHEAPHYSICSFABRIC_H
#define RHEA_RHEAPHYSICSFABRIC_H

#include <Rhea/Application.h>
#include <Rhea/ModuleSystem.h>

namespace Rhea
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
	class RheaPhysicsFabric : public Rhea::IFabric
	{
	public:
		void Register(Application* app) override;

		const char* getName() const override { return "RheaPhysics"; }
	};
}

#endif // !RHEA_RHEAPHYSICS_H
