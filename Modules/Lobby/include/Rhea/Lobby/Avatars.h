#ifndef MEWORLA_LOBBY_AVATARS_H
#define MEWORLA_LOBBY_AVATARS_H

#include <Rhea/Application.h>
#include <Rhea/Fabrics/Network/ClientService.h>
#include <Rhea/Fabrics/GUI/include/Rhea/GUI/Widget.h>
#include <ClientConnect.h>

namespace Meworla
{
	class Avatars : public Rhea::Unit
	{
		MECLASS(Avatars, Unit)
	public:
		Avatars(std::string path_name, std::string type_name) :
			m_net(nullptr),
			m_connector(nullptr),
			m_avatar_w(nullptr),
			m_newavatar_w(nullptr),
			Rhea::Unit(path_name, type_name)
		{}

		// initialization
		bool onDisplay(Rhea::Event* ev, Rhea::Unit* unit);
		void ready();		

		// selection
		bool onOpen(Rhea::Event* ev, Rhea::Unit* unit);
		bool onCallCreate(Rhea::Event* ev, Rhea::Unit* unit);

		// for new avatar
		bool onSetAvatarname(Rhea::Event* ev, Rhea::Unit* unit);
		bool onSetAvatarType(Rhea::Event* ev, Rhea::Unit* unit);
		bool onCreate(Rhea::Event* ev, Rhea::Unit* unit);
		bool onCreateCancel(Rhea::Event* ev, Rhea::Unit* unit);
		bool onCreateFailed(Rhea::Event* ev, Rhea::Unit* unit);

		// entering world
		bool onWorldEnter(Rhea::Event* ev, Rhea::Unit* unit);

	protected:
		Rhea::ClientService* m_net;
		Meworla::ClientConnect* m_connector;

		Rhea::Widget* m_avatar_w;
		Rhea::Widget* m_newavatar_w;

		std::string m_avatar_name;
		std::string m_avatar_type;
	};
}

#endif // !MEWORLA_LOBBY_AVATARS_H
