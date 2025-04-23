#ifndef MEWORLA_LOBBY_H
#define MEWORLA_LOBBY_H

#include <Rhea/Application.h>
#include <Rhea/ModuleSystem.h>

namespace Meworla
{

	class Lobby : public Rhea::IFabricModule		
	{
	public:

		// fabrics functions
		static void Register(Rhea::Application* app);

		// get name
		static std::string getName() { return "Lobby"; }
	};
}

#endif // MEWORLA_LOBBY_H