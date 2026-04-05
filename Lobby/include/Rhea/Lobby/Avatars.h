#ifndef MEWORLA_LOBBY_AVATARS_H
#define MEWORLA_LOBBY_AVATARS_H

#include <Rhea/Application.h>
#include <Rhea/Fabrics/Network/ClientService.h>
#include <Rhea/GUI/Widget.h>
#include <Rhea/Lobby/ClientConnect.h>

namespace Meworla
{
	class Avatars : public rhea::Unit
	{
		MECLASS(Avatars, Unit)
	public:
		Avatars(std::string path_name, std::string type_name) :
			m_net(nullptr),
			m_connector(nullptr),
			m_avatar_w(nullptr),
			m_newavatar_w(nullptr),
			rhea::Unit(path_name, type_name)
		{}

		// initialization
		bool onDisplay(rhea::Event* ev, rhea::Unit* unit);
		void ready();		

		// selection
		bool onOpen(rhea::Event* ev, rhea::Unit* unit);
		bool onCallCreate(rhea::Event* ev, rhea::Unit* unit);

		// for new avatar
		bool onSetAvatarname(rhea::Event* ev, rhea::Unit* unit);
		bool onSetAvatarType(rhea::Event* ev, rhea::Unit* unit);
		bool onCreate(rhea::Event* ev, rhea::Unit* unit);
		bool onCreateCancel(rhea::Event* ev, rhea::Unit* unit);
		bool onCreateFailed(rhea::Event* ev, rhea::Unit* unit);

		// entering world
		bool onWorldEnter(rhea::Event* ev, rhea::Unit* unit);

	protected:
		Rhea::ClientService* m_net;
		Meworla::ClientConnect* m_connector;

		rhea::Widget* m_avatar_w;
		rhea::Widget* m_newavatar_w;

		std::string m_avatar_name;
		std::string m_avatar_type;
	};
}

#endif // !MEWORLA_LOBBY_AVATARS_H
