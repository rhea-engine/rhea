#ifndef RHEA_CHARACTERFABRIC_H
#define RHEA_CHARACTERFABRIC_H

#include <Rhea/Application.h>
#include <Rhea/FabricSystem.h>

namespace rhea
{
	/** Fabrics for characters
	*/
	class CharacterFabric : public rhea::IFabric
	{
	public:
		void Register(Application* app) override;

		const char* getName() const override { return "Character"; }
	};
}

#endif // !RHEA_CHARACTERFABRICS_H
