#include "BasicController.h"
#include "Item.h"
#include "Poseidon/Node.h"
#include "../Application.h"
#include "../UnitTree.h"

#include "EventInput.h"

namespace Rhea
{
	bool BasicController::onInput(Event* ev, Unit* unit)
	{
		// safety check
		if (!m_target)
			return false;

		EventInput* e = (EventInput*)ev;

		// default speed in m/s
		float move_step = 1.66f; // ~6km/h 
		float turn_step = 0.3f;
		float side_step = 0.575f;
		float up_step = 0.5f;

		if (e->isAction("move_left")) {
			m_move.x = -side_step;
		}
		else if (e->isAction("move_right")) {
			m_move.x = side_step;
		}
		else if (e->isAction("move_forward")) {
			m_move.z = move_step;			
		}
		else if (e->isAction("move_back")) {
			m_move.z = -move_step;			
		}
		else if (e->isAction("move_up")) {
			m_move.y = up_step;
		}
		else if (e->isAction("move_down")) {
			m_move.y = -up_step;
		}
		if (e->isAction("move_left_stop")) {
			m_move.x = 0;
		}
		else if (e->isAction("move_right_stop")) {
			m_move.x = 0;
		}
		else if (e->isAction("move_forward_stop")) {
			m_move.z = 0;
		}
		else if (e->isAction("move_back_stop")) {
			m_move.z = 0;
		}
		else if (e->isAction("move_up_stop")) {
			m_move.y = 0;
		}
		else if (e->isAction("move_down_stop")) {
			m_move.y = 0;
		}
		else if (e->isAction("turn_left")) {
			m_turn = -turn_step;
		}
		else if (e->isAction("turn_right")) {
			m_turn = turn_step;
		}
		else if (e->isAction("turn_left_stop"))
			m_turn = 0;
		else if (e->isAction("turn_right_stop"))
			m_turn = 0;
		else if (e->isAction("jump"))
			m_target->jump();

		m_target->setMovement(m_move);
		m_target->setTurn(m_turn);

		return false;
	}
	
	void BasicController::connect(Item* target)
	{
		m_target = target;
		Poseidon::Node* node = m_target->getNode();
		if (!node) {
			m_app->log("Basic Controller: target has no node");
			m_target = nullptr;
			return;
		}
		m_position = *node->get_derived_position();
		m_orientation = m_position.get_rotation();
	}

	void BasicController::show(Poseidon::Core* core)
	{
		auto target_it = m_values.find("target");
		Unit* target;
		if (target_it != m_values.end()) {
			target = m_app->getUnitTree()->find(target_it->second);
			if (target)
				// TODO: safety check
				connect(static_cast<Item*>(target));
		}
		_registerEvent(m_app->getUnitTree());
	}

	void BasicController::message(std::string method_name)
	{
		Unit::message(method_name);
		if (method_name == "show")
			show(m_app->get_core());
	}

	void BasicController::setPosition(Poseidon::Vector3 position)
	{
		m_position.setPosition(position);
		//m_position.setOrientation(Poseidon::Quaternion(m_rotation, Poseidon::Vector3(0.0f, 1.0f, 0.0f))); 
		((Item*)m_target)->setPosition(m_position);
	}

	void BasicController::lookAt(Poseidon::Vector3& target)
	{
		m_position.lookAt(target, Poseidon::Vector3::UNIT_Z);
		((Item*)m_target)->setPosition(m_position);
	}

	Poseidon::Vector3 BasicController::getPosition()
	{
		if (m_target)
			return m_target->getNode()->get_derived_position()->get_worldspace();
		return m_position.get_worldspace();
	}

	void BasicController::logMe()
	{
		Unit::logMe();
		printf("Target: %s\n", (m_target) ? m_target->getPathName().c_str() : "(empty)");
		printf("Position: (%f,%f,%f)\n", m_position.get_worldspace().x, m_position.get_worldspace().y, m_position.get_worldspace().z);
	}

	void BasicController::_registerEvent(Unit* unittree)
	{
		unittree->registerEventHandler("unhandled_input", std::bind(&BasicController::onInput, this, std::placeholders::_1, std::placeholders::_2));
	}

	Unit* BasicController::createType(std::string name, std::string type)
	{
		Unit* ret = (Unit*)new BasicController(name, type);
		return ret;
	}	
}