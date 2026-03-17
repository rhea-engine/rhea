#ifndef RHEA_TERRAINFABRIC_H
#define RHEA_TERRAINFABRIC_H

#include <Rhea/Application.h>
#include <Rhea/FabricSystem.h>

namespace Rhea
{
	/** Fabrics for terrain handling
	*/
	class TerrainFabric : public Rhea::IFabric
	{
	public:
		void Register(Application* app) override;

		const char* getName() const override { return "Terrain"; }
	};
}

#endif // !RHEA_TERRAINFABRIC_H
