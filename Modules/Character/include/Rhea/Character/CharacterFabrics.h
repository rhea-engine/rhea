#ifndef RHEA_CHARACTERFABRICS_H
#define RHEA_CHARACTERFABRICS_H

#include <Rhea/Application.h>
#include <Rhea/ModuleSystem.h>

namespace Rhea
{
	/** Fabrics for basic image loading
	*/
	class CharacterFabrics : public Rhea::IFabricModule
	{
	public:
		void Register(Application* app);

		const char* getName() const override { return "Character"; }
	};
}

#endif // !RHEA_CHARACTERFABRICS_H
