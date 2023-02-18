#ifndef POSEIDON_WORLDCLIENT_H
#define POSEIDON_WORLDCLIENT_H

#include <Rhea/Unit.h>
#include <Hermes/ClientStream.h>
#include <Rhea/Fabrics/Core/Scene.h>
#include <Rhea/Components/NetworkDelegate.h>
#include <Math/Vector3.h>

namespace Poseidon
{
	class WorldClient : public Rhea::Unit, public Rhea::NetworkDelegate
	{
	public:
		WorldClient(std::unique_ptr<Hermes::ClientStream> hermes) :
			m_onWorldHandler(nullptr),
			m_scene(nullptr),
			m_player_set(false),
			Unit("worldclient", "//meworla/worldclient")
		{
			m_hermes = std::move(hermes);
		}

		// login credentials
		void setAuth(std::string avatar);

		// startup
		typedef std::function<void(int)> OnWorldHandler;
		void start(Rhea::Scene* scene, OnWorldHandler handler);

		// stop the connection
		void stop();

		// NetworkDelegate functions
		void changeMovement(int nuid, Math::Vector3 move);
		void changeTurn(int nuid, float turn);

	protected:
		std::unique_ptr<Hermes::ClientStream> m_hermes;
		Rhea::Scene* m_scene;
		OnWorldHandler m_onWorldHandler;
		std::string m_avatar;
		std::map<int,Rhea::Item*> m_net_items;
		bool m_player_set;
	};
}

#endif // !POSEIDON_WORLDCLIENT_H
