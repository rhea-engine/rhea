#include "BasicController.h"
#include "Item.h"
#include "Poseidon/Node.h"
#include "../Application.h"
#include "../UnitTree.h"

namespace Rhea
{
	void BasicController::onInput(std::string action, std::shared_ptr<Psio::InputEvent>& evt)
	{
		// safety check
		if (!m_target)
			return;

		// default speed in m/s
		float move_step = 1.66f; // ~6km/h 
		float turn_step = 0.3f;
		float side_step = 0.575f;
		float up_step = 0.5f;

		if (action == "move_left") {
			m_move.x = -side_step;
		}
		else if (action == "move_right") {
			m_move.x = side_step;
		}
		else if (action == "move_forward") {
			m_move.z = move_step;			
		}
		else if (action == "move_back") {
			m_move.z = -move_step;			
		}
		else if (action == "move_up") {
			m_move.y = up_step;
		}
		else if (action == "move_down") {
			m_move.y = -up_step;
		}
		if (action == "move_left_stop") {
			m_move.x = 0;
		}
		else if (action == "move_right_stop") {
			m_move.x = 0;
		}
		else if (action == "move_forward_stop") {
			m_move.z = 0;
		}
		else if (action == "move_back_stop") {
			m_move.z = 0;
		}
		else if (action == "move_up_stop") {
			m_move.y = 0;
		}
		else if (action == "move_down_stop") {
			m_move.y = 0;
		}
		else if (action == "turn_left") {
			m_turn = -turn_step;
		}
		else if (action == "turn_right") {
			m_turn = turn_step;
		}
		else if (action == "turn_left_stop")
			m_turn = 0;
		else if (action == "turn_right_stop")
			m_turn = 0;
	}

	void BasicController::onPhysics(float delta)
	{
		bool  dirty = false;
		Poseidon::Vector3 move(m_move);
		if (move != Poseidon::Vector3::ZERO) {
			// currently moving			
			move *= delta;
			m_position.moveRelative(move);
			dirty = true;
		}
		if (m_turn != 0) {
			float turn_step = m_turn * delta;
			Poseidon::Quaternion q;
			q.fromAngleAxis(turn_step, Poseidon::Vector3(0, 1, 0));
			m_orientation = q * m_orientation;
			m_orientation.normalise();
			m_position.setOrientation(m_orientation);
			dirty = true;
		}
		if (dirty && m_target)
			((Item*)m_target)->setPosition(m_position);
	}

	void BasicController::connect(Unit* target)
	{
		m_target = target;
		m_position = *((Item*)m_target)->getNode()->get_derived_position();
		m_orientation = m_position.get_rotation();
	}

	void BasicController::show(Poseidon::Core* core)
	{
		auto target_it = m_values.find("target");
		Unit* target;
		if (target_it != m_values.end()) {
			target = m_app->getUnitTree()->find(target_it->second);
			if (target)
				connect(target);
		}
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

	void BasicController::logMe()
	{
		Unit::logMe();
		printf("Target: %s\n", (m_target) ? m_target->getPathName().c_str() : "(empty)");
		printf("Position: (%f,%f,%f)\n", m_position.get_worldspace().x, m_position.get_worldspace().y, m_position.get_worldspace().z);
	}

	Unit* BasicController::createType(std::string name, std::string type)
	{
		Unit* ret = (Unit*)new BasicController(name, type);
		return ret;
	}	
}