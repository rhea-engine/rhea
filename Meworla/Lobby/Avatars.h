#ifndef MEWORLA_LOBBY_AVATARS_H
#define MEWORLA_LOBBY_AVATARS_H

#include <Rhea/Application.h>
#include <Rhea/Fabrics/Network/ClientService.h>

namespace Meworla
{
	class Avatars : public Rhea::Unit
	{
	public:
		Avatars(std::string path_name, std::string type_name) :
			m_net(nullptr),
			Rhea::Unit(path_name, type_name)
		{}

		// initialization
		bool onDisplay(Rhea::Event* ev, Rhea::Unit* unit);
		void ready();

		// selection
		bool onOpen(Rhea::Event* ev, Rhea::Unit* unit);

		// entering world
		bool onWorldEnter(Rhea::Event* ev, Rhea::Unit* unit);
		void enterWithAvatar(Rhea::Scene* world_scene, std::string avatar_name);

		// constructor
		static Rhea::Unit* createType(std::string name, std::string type);

	protected:
		Rhea::ClientService* m_net;
	};
}

#endif // !MEWORLA_LOBBY_AVATARS_H
