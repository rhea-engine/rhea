#ifndef MEWORLA_LOBBYFABRIC_H
#define MEWORLA_LOBBYFABRIC_H

#include <Rhea/Application.h>
#include <Rhea/FabricSystem.h>

namespace Meworla
{

	class LobbyFabric : public rhea::IFabric
	{
	public:

		// fabrics functions
		void Register(rhea::Application* app) override;

		// get name
		const char* getName() const override { return "Lobby"; }
	};
}

#endif // MEWORLA_LOBBYFABRIC_H