#ifndef RHEA_SKYBOXFABRIC_H
#define RHEA_SKYBOXFABRIC_H

#include <Rhea/Application.h>
#include <Rhea/FabricSystem.h>

namespace Rhea
{
	/** Fabrics for skybox handling
	*/
	class SkyboxFabric : public Rhea::IFabric
	{
	public:
		void Register(Application* app) override;

		const char* getName() const override { return "Skybox"; }
	};
}

#endif // !RHEA_SKYBOXFABRIC_H
