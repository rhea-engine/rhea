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

namespace Rhea
{
	class WorldClient : public Rhea::NetworkDelegate
	{
	public:
		WorldClient(Rhea::Application* app, Hermes::Stream* stream) :
			m_scene(nullptr),
			m_player_set(false),
			m_in_world(false),
			m_stream(stream),
			m_app(app)
		{}

		typedef std::function<void(int err)> CompletionHandler;

		// startup
		void Initialize();
		void setScene(Rhea::Scene* scene) { m_scene = scene; }
		
		// event handling
		void onWorldEnter(CompletionHandler handler) { m_on_enter_world = handler; }

		// start/stop the world state
		void Start() { m_in_world = true; }
		void Stop();

		// NetworkDelegate functions
		void changeMovement(int nuid, Math::Vector3 move);
		void changeTurn(int nuid, float turn);

	protected:
		Hermes::Stream* m_stream;
		Rhea::Scene* m_scene;
		std::string m_avatar;
		std::map<int,Rhea::Item*> m_net_items;
		bool m_player_set;
		bool m_in_world;
		CompletionHandler m_on_enter_world;
		Rhea::Application* m_app;

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
