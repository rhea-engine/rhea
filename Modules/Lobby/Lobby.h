#ifndef MEWORLA_LOBBY_H
#define MEWORLA_LOBBY_H

#include <Rhea/Unit.h>

namespace Meworla
{

	class Lobby : public Rhea::Unit		
	{
		MECLASS(Lobby, Unit)
	public:
		Lobby(std::string path_name, std::string type_name) :
			Rhea::Unit(path_name, type_name)
		{}

		void ready();

		// fabrics functions
		static void registerFabrics(Rhea::Application* app);
	};
}

#endif // MEWORLA_LOBBY_H