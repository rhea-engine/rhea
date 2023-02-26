#include "ServerService.h"
#include <Psio/Log.h>
#include <Hermes/ServerStream.h>
#include <Rhea/Application.h>
#include <Rhea/UnitTree.h>
#include <Rhea/Fabrics/3D/Item.h>
#include <Hermes/Messages/MessagePosition.h>
#include <Hermes/Messages/MessageDestroy.h>
#include <Rhea/Physics/PhysicsService.h>
#include <Rhea/Platform/PlatformService.h>

void ServerService::setup(Rhea::Application* app, std::unordered_map<std::string, std::string> attributes, std::unordered_map<std::string, Variant> values)
{
	Service::setup(app, attributes, values);

	ip = "127.0.0.1";
	port = 4242;

	if (m_values.find("ip") != m_values.end())
		ip = m_values["ip"];
	if (m_values.find("port") != m_values.end())
		port = (int)m_values["port"];

	// get network from services
	m_network = static_cast<Rhea::PlatformService*>(m_app->getService("platform"))->getEngine()->getNet();
}

void ServerService::ready()
{
	if (!m_network) {
		LOG(LOG_FATAL) << "Network service not found";
		return;
	}
	m_server_sock = std::make_unique<Psio::TCPSocket>(m_network);

	int err = m_server_sock->Server(ip, port);
	LOG(LOG_INFO) << "Started Server. Listening on " << ip << ":" << port;

	// fill message factory
	m_msg_factory = std::make_unique<Hermes::MessageFactory>();
	m_msg_factory->registerCoreMessages();

	m_server_sock->Accept([&](Psio::StreamSocket* new_sock, int err) {
		if (err < 0) {
			LOG(LOG_ERROR) << "OnConnection failed: " << err;
			return;
		}
		else {
			std::unique_ptr<Psio::StreamSocket> sock;
			sock.reset(new_sock);

			std::unique_ptr<Hermes::ServerStream> hermes = std::make_unique<Hermes::ServerStream>(std::move(sock), m_msg_factory.get());
			ClientConnection* conn = new ClientConnection(this->m_app, m_scene, std::move(hermes));
			m_connections.push_back(conn);
			int ret = conn->Start([&,conn](int err) {
				if (err < -1) {
					DLOG(LOG_DEBUG) << "Connection error: " << err;
					// for now simply close the connection
					disconnect(conn->getAuthItem());
				}
				});
			
			if (ret < -1) {
				LOG(LOG_ERROR) << "could not start client connection: " << ret;
			}
		}
		});
	if (err < 0) {
		LOG(LOG_FATAL) << "failed to start server";
		return;
	}
}

bool ServerService::onFrameEnd()
{
	// iterate over items and send updates
	if (m_scene) {
		std::vector<Rhea::Item*> items = m_scene->getItems();
		for (auto item_it = items.begin(); item_it != items.end(); ++item_it) {
			Rhea::NetworkComponent* net_com = (*item_it)->getNetworkComponent();
			if (net_com && net_com->isDirty()) {
				Hermes::MessagePosition msg;
				msg.NUID = net_com->NUID();
				Math::Vector3 pos = (*item_it)->getTransform()->get_position()->get_worldspace();
				msg.setPosition(pos.x, pos.y, pos.z);
				Math::Quaternion rot = (*item_it)->getTransform()->get_position()->get_rotation();
				msg.setTransform(rot.w, rot.x, rot.y, rot.z);

				// send to all clients
				for (auto it = m_connections.begin(); it != m_connections.end(); ++it) {
					ClientConnection* stream = *it;
					int ret = stream->sendMessage(&msg);
					if (ret < -1) {
						// network error - stop the player
						disconnect(stream->getAuthItem()); 
					}
				}
				net_com->clear();
			}
		}
	}
	return false;
}

Rhea::Unit* ServerService::createType(std::string name, std::string type)
{
	Unit* ret = (Unit*)new ServerService(name, type);
	return ret;
}

void ServerService::broadcast(Hermes::Message* msg, ClientConnection* source)
{
	// send to all clients
	for (auto it = m_connections.begin(); it != m_connections.end(); ++it) {
		ClientConnection* stream = *it;
		if (stream == source)
			continue;
		int ret = stream->sendMessage(msg);
		if (ret < -1) {
			// network error - stop the player
			disconnect(stream->getAuthItem());
		}
	}
}

void ServerService::disconnect(Rhea::Item* item)
{
	if (item) {
		// cancel all movement
		item->setMovement(Math::Vector3::ZERO);
		item->setTurn(0.0f);
		// remove item from scene?
		// TODO make this an option
		Rhea::NetworkComponent* net_com = item->getNetworkComponent();
		if (net_com) {
			Hermes::MessageDestroy msg;
			msg.NUID = net_com->NUID();
			broadcast(&msg, nullptr);
			// remove from scene
			Rhea::Service* phy_srv = m_app->getService("physics");
			// add to physics					
			m_scene->removeItem(item);
			m_scene->removeChild(item);
			if (phy_srv) {
				((Rhea::PhysicsService*)phy_srv)->removeItem(item);
			}
		}
	}
}
