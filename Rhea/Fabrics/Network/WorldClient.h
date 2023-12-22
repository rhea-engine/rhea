#ifndef RHEA_WORLDCLIENT_H
#define RHEA_WORLDCLIENT_H

#include <Rhea/Unit.h>
#include <Hermes/ClientStream.h>
#include <Rhea/Fabrics/Core/Scene.h>
#include "NetworkDelegate.h"
#include <Math/Vector3.h>
#include <Hermes/Messages/MessageAvatarList.h>
#include <Hermes/Messages/MessageCreate.h>
#include <Hermes/Messages/MessagePosition.h>
#include <Hermes/Messages/MessageDestroy.h>

namespace Rhea
{
	class WorldClient : public Rhea::Unit, public Rhea::NetworkDelegate
	{
		MECLASS(WorldClient, Rhea::Unit)
	public:
		WorldClient(std::string name, std::string type_name) :
			m_scene(nullptr),
			m_player_set(false),
			m_in_world(false),
			Unit(name, type_name)
		{}
		WorldClient() :
			WorldClient("worldclient", "//meworla/worldclient")
		{}

		typedef std::function<void(int err)> CompletionHandler;

		// set hermes
		void setStream(std::unique_ptr<Hermes::ClientStream> hermes) {
			m_hermes = std::move(hermes);
		}

		// login credentials
		void setAuth(std::string method, std::string username, std::vector<std::string> secrets);

		// startup
		void Open(std::string worldname);
		void Start();
		void setScene(Rhea::Scene* scene) { m_scene = scene; }
		void Register(std::string method, std::string username, std::string name, std::string ext_id, std::vector<std::string> secrets);
		void CreateAvatar(std::string name, std::string type_name);

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
		std::string m_method;
		std::string m_username;
		std::vector<std::string> m_secrets;
		std::map<int,Rhea::Item*> m_net_items;
		bool m_player_set;
		bool m_in_world;

		// handle messages
		void onLogin();
		void onAvatarlistMessage(Hermes::MessageAvatarList* msg);
		void onCreateAvatarFailedMessage(Hermes::Message* msg);
		void onEnterWorldFailedMessage(Hermes::Message* msg);
		void onCreateMessage(Hermes::MessageCreate* msg);
		void onPositionMessage(Hermes::MessagePosition* msg);
		void onDestroyMessage(Hermes::MessageDestroy* msg);
	};
}

#endif // !RHEA_WORLDCLIENT_H
