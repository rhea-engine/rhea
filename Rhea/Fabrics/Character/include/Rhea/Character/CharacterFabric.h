#ifndef RHEA_CHARACTERFABRIC_H
#define RHEA_CHARACTERFABRIC_H

#include <Rhea/Application.h>
#include <Rhea/FabricSystem.h>

namespace Rhea
{
	/** Fabrics for characters
	*/
	class CharacterFabric : public Rhea::IFabric
	{
	public:
		void Register(Application* app) override;

		const char* getName() const override { return "Character"; }
	};
}

#endif // !RHEA_CHARACTERFABRICS_H
