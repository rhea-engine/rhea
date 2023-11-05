#ifndef RHEA_WORLDCLIENT_H
#define RHEA_WORLDCLIENT_H

#include <Rhea/Unit.h>
#include <Hermes/ClientStream.h>
#include <Rhea/Fabrics/Core/Scene.h>
#include "NetworkDelegate.h"
#include <Math/Vector3.h>

namespace Rhea
{
	class WorldClient : public Rhea::Unit, public Rhea::NetworkDelegate
	{
		MECLASS(WorldClient, Rhea::Unit)
	public:
		WorldClient(std::string name, std::string type_name) :
			m_scene(nullptr),
			m_player_set(false),
			Unit(name, type_name)
		{}
		WorldClient() :
			WorldClient("worldclient", "//meworla/worldclient")
		{}

		// set hermes
		void setStream(std::unique_ptr<Hermes::ClientStream> hermes) {
			m_hermes = std::move(hermes);
		}

		// login credentials
		void setAuth(std::string world, std::string username);

		// startup
		void start();
		void setScene(Rhea::Scene* scene) { m_scene = scene; }

		// stop the connection
		void stop();

		// special messages
		void enterWorld(std::string avatar_name);

		// NetworkDelegate functions
		void changeMovement(int nuid, Math::Vector3 move);
		void changeTurn(int nuid, float turn);

	protected:
		std::unique_ptr<Hermes::ClientStream> m_hermes;
		Rhea::Scene* m_scene;
		std::string m_avatar;
		std::string m_world;
		std::string m_username;
		std::map<int,Rhea::Item*> m_net_items;
		bool m_player_set;
	};
}

#endif // !RHEA_WORLDCLIENT_H
