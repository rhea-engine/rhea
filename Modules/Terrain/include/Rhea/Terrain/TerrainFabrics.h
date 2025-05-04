#ifndef RHEA_TERRAINFABRICS_H
#define RHEA_TERRAINFABRICS_H

#include <Rhea/Application.h>
#include <Rhea/ModuleSystem.h>

namespace Rhea
{
	/** Fabrics for basic image loading
	*/
	class TerrainFabrics : public Rhea::IFabricModule
	{
	public:
		void Register(Application* app);

		const char* getName() const override { return "Terrain"; }
	};
}

#endif // !RHEA_TERRAINFABRICS_H
