#ifndef MEWORLA_LOBBYFABRIC_H
#define MEWORLA_LOBBYFABRIC_H

#include <Rhea/Application.h>
#include <Rhea/ModuleSystem.h>

namespace Meworla
{

	class LobbyFabric : public Rhea::IFabric
	{
	public:

		// fabrics functions
		void Register(Rhea::Application* app) override;

		// get name
		const char* getName() const override { return "Lobby"; }
	};
}

#endif // MEWORLA_LOBBYFABRIC_H