#ifndef MEWORLA_LOBBYFABRICS_H
#define MEWORLA_LOBBYFABRICS_H

#include <Rhea/Application.h>
#include <Rhea/ModuleSystem.h>

namespace Meworla
{

	class LobbyFabrics : public Rhea::IFabricModule		
	{
	public:

		// fabrics functions
		void Register(Rhea::Application* app) override;

		// get name
		const char* getName() const override { return "Lobby"; }
	};
}

#endif // MEWORLA_LOBBYFABRICS_H