#ifndef MEWORLA_LOBBY_CONNECT_H
#define MEWORLA_LOBBY_CONNECT_H

#include <Rhea/Application.h>
#include <Rhea/Fabrics/Network/ClientService.h>
#include <Rhea/Fabrics/GUI/Widget.h>


namespace Meworla
{
	class ClientConnect : public Rhea::Unit
	{
		MECLASS(ClientConnect, Unit)
	public:
		ClientConnect(std::string path_name, std::string type_name) :
			m_connection_state(0),
			m_error(0),
			m_net(nullptr),
			m_info_w(nullptr),
			m_error_w(nullptr),
			Rhea::Unit(path_name, type_name)
		{}

		// start the connection
		bool onDisplay(Rhea::Event* ev, Rhea::Unit* unit);

		// abort connection
		bool onAbort(Rhea::Event* ev, Rhea::Unit* unit);

		// retry connection
		bool onRetry(Rhea::Event* ev, Rhea::Unit* unit);


		void doConnect();
		void doError();
		void ready();

	protected:
		int m_connection_state;
		int m_error;
		Rhea::ClientService* m_net;
		Rhea::Widget* m_info_w;
		Rhea::Widget* m_error_w;
	};
}

#endif // !MEWORLA_LOBBY_CONNECT_H
