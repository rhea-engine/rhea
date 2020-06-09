#include "PhysicsService.h"
#include "Body.h"
#include "../Controller/BasicController.h"
#include "../Item.h"
#include "../Application.h"
#include "PhysicsEvent.h"

namespace Rhea
{
	PhysicsService::PhysicsService(Application* app) :
		m_item(nullptr),
		m_terrain(nullptr),
		Service(app, "PhysicsService")
	{

	}

	PhysicsService::PhysicsService(Application* app, std::string service_name) : 
		m_item(nullptr),
		m_terrain(nullptr),
		Service(app, service_name)
	{
	}

	void PhysicsService::step(float delta)
	{
		// send pre event
		std::unique_ptr<Rhea::PhysicsEvent> ev = std::make_unique<Rhea::PhysicsEvent>("physics_pre", delta, m_terrain);
		sendEvent(std::string("pre_physics"), ev.get());

		if (!m_item)
			return;

		bool  dirty = false;
		Poseidon::Vector3 move(m_item->getMovement());
		if (move != Poseidon::Vector3::ZERO) {
			// currently moving			
			move *= delta;
			m_item->getNode()->get_position()->moveRelative(move);			
			dirty = true;
		}
		float m_turn = m_item->getTurn();
		if (m_turn != 0) {
			float turn_step = m_turn * delta;
			Poseidon::Quaternion q;
			q.fromAngleAxis(turn_step, Poseidon::Vector3(0, 1, 0));
			Poseidon::Quaternion m_orientation = q * m_item->getNode()->get_position()->get_rotation(); //* m_orientation;
			m_orientation.normalise();			
			m_item->getNode()->get_position()->setOrientation(m_orientation);
			dirty = true;
		}
	}

	void PhysicsService::setScene(Scene* scene)
	{
		Controller* controller = static_cast<Controller*>(scene->findByType("//meworla/controller"));
		if (controller) {
			// get target from controller
			m_item = controller->getTarget();

			PhysicsBody* body = new PhysicsBody(m_item);
			m_item->addElement("physics", body);
		}
		else {
			m_app->log("No controller found in setting scene");
		}
	}
	
	void PhysicsService::onInit()
	{
	}
}