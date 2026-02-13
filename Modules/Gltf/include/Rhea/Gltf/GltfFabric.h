#ifndef RHEA_GLTFFABRIC_H
#define RHEA_GLTFFABRIC_H

#include <Rhea/Application.h>
#include <Rhea/ModuleSystem.h>

namespace Rhea
{	
	/** Fabrics for gltf file loading
	*/
	class GltfFabric : public Rhea::IFabric
	{
	public:
		void Register(Application* app) override;

		const char* getName() const override { return "Gltf"; }
	};
}

#endif // !RHEA_GLTF_H
