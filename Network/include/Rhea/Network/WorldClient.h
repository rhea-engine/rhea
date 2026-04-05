#ifndef RHEA_WORLDCLIENT_H
#define RHEA_WORLDCLIENT_H

#include <Rhea/Unit.h>
#include <Hermes/ClientStream.h>
#include <Rhea/Core/Scene.h>
#include "NetworkDelegate.h"
#include <Math/Vector3.h>
#include <Hermes/Messages/MessageAvatarList.h>
#include <Hermes/Messages/MessageCreate.h>
#include <Hermes/Messages/MessagePosition.h>
#include <Hermes/Messages/MessageDestroy.h>

namespace rhea
{
	class WorldClient : public rhea::NetworkDelegate
	{
	public:
		WorldClient(rhea::Application* app, hermes::Stream* stream) :
			m_scene(nullptr),
			m_player_set(false),
			m_in_world(false),
			m_stream(stream),
			m_app(app)
		{}

		typedef std::function<void(int err)> CompletionHandler;

		// startup
		void Initialize();
		void setScene(rhea::Scene* scene) { m_scene = scene; }
		
		// event handling
		void onWorldEnter(CompletionHandler handler) { m_on_enter_world = handler; }

		// start/stop the world state
		void Start() { m_in_world = true; }
		void Stop();

		// NetworkDelegate functions
		void changeMovement(int nuid, math::Vector3 move);
		void changeTurn(int nuid, float turn);

	protected:
		hermes::Stream* m_stream;
		rhea::Scene* m_scene;
		std::string m_avatar;
		std::map<int,rhea::Item*> m_net_items;
		bool m_player_set;
		bool m_in_world;
		CompletionHandler m_on_enter_world;
		rhea::Application* m_app;

		// handle messages
		void onLogin();
		void onAvatarlistMessage(hermes::MessageAvatarList* msg);
		void onCreateAvatarFailedMessage(hermes::Message* msg);
		void onEnterWorldFailedMessage(hermes::Message* msg);
		void onCreateMessage(hermes::MessageCreate* msg);
		void onPositionMessage(hermes::MessagePosition* msg);
		void onDestroyMessage(hermes::MessageDestroy* msg);
	};
}

#endif // !RHEA_WORLDCLIENT_H
