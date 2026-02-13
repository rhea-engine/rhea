#ifndef RHEA_TERRAINFABRICS_H
#define RHEA_TERRAINFABRICS_H

#include <Rhea/Application.h>
#include <Rhea/ModuleSystem.h>

namespace Rhea
{
	/** Fabrics for terrain handling
	*/
	class TerrainFabrics : public Rhea::IFabricModule
	{
	public:
		void Register(Application* app) override;

		const char* getName() const override { return "Terrain"; }
	};
}

#endif // !RHEA_TERRAINFABRICS_H
