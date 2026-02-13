#ifndef RHEA_GLTF_H
#define RHEA_GLTF_H

#include <Rhea/Application.h>
#include <Rhea/ModuleSystem.h>

namespace Rhea
{	
	/** Fabrics for gltf file loading
	*/
	class GltfFabrics : public Rhea::IFabricModule
	{
	public:
		void Register(Application* app) override;

		const char* getName() const override { return "Gltf"; }
	};
}

#endif // !RHEA_GLTF_H
