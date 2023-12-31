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

namespace Rhea
{

	void ServerService::setup(Rhea::Application* app, std::unordered_map<std::string, std::string> attributes, std::unordered_map<std::string, Variant> values)
	{
		Service::setup(app, attributes, values);

		ip = "127.0.0.1";
		m_port = 4242;

		if (m_values.find("ip") != m_values.end())
			ip = m_values["ip"];
		if (m_values.find("port") != m_values.end())
			m_port = (int)m_values["port"];

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

		int err = m_server_sock->Server(ip, m_port);
		LOG(LOG_INFO) << "Started Server. Listening on " << ip << ":" << m_port;

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

				std::unique_ptr<Hermes::Stream> stream = std::make_unique<Hermes::Stream>(std::move(sock), m_msg_factory.get());					
				ClientConnection* connection = new ClientConnection(this->m_app, m_scene, std::move(stream));
				if (m_on_connection) {
					m_on_connection(connection);
				}

				// handle closing of connection
					// closing handler
				connection->onClosed([&, connection](int err) {
					LOG(LOG_INFO) << "Connection closed with err=" << err;
					disconnect(connection);
					});

				m_connections.push_back(connection);
				int ret = connection->Start([&, connection](int err) {
					if (err < -1) {
						DLOG(LOG_DEBUG) << "Connection error: " << err;
					}
					else
						DLOG(LOG_DEBUG) << "Connection closed";
					// for now simply close the connection
					disconnect(connection);
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
						ClientConnection* connection = *it;
						int ret = connection->sendMessage(&msg);
						if (ret < -1) {
							// network error - stop the player
							disconnect(connection);
						}
					}
					net_com->clear();
				}
			}
		}
		return false;
	}

	void ServerService::broadcast(Hermes::Message* msg, ClientConnection* source)
	{
		// send to all clients
		for (auto it = m_connections.begin(); it != m_connections.end(); ++it) {
			ClientConnection* connection = *it;
			if (connection == source)
				continue;
			int ret = connection->sendMessage(msg);
			if (ret < -1) {
				// network error - stop the player
				disconnect(connection);
			}
		}
	}

	void ServerService::disconnect(ClientConnection* connection)
	{
		Rhea::Item* item = connection->getAuthItem();
		// prevent from double disconnect
		connection->setAuthItem(nullptr);

		// remove item from scene
		if (item) {
			LOG(LOG_INFO) << "Disconnecting avatar: " << item->getPathName();
			// cancel all movement
			item->setMovement(Math::Vector3::ZERO);
			item->setTurn(0.0f);
			// remove item from scene?
			// TODO make this an option
			Rhea::NetworkComponent* net_com = item->getNetworkComponent();
			if (net_com) {
				Hermes::MessageDestroy msg;
				msg.NUID = net_com->NUID();
				broadcast(&msg, connection);
				// remove from scene
				Rhea::Service* phy_srv = m_app->getService("physics");
				// add to physics					
				m_scene->removeItem(item);
				m_scene->removeChild(item);
				if (phy_srv) {
					((Rhea::PhysicsService*)phy_srv)->removeItem(item);
				}
			}
			delete item;
		}

		// remove from list
		m_connections.erase(std::remove(m_connections.begin(), m_connections.end(), connection), m_connections.end());
		// delete connection
		delete connection;
	}
}