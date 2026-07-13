/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#include <Rhea/Controller/BasicController.h>
#include <Rhea/Core/Item.h>
#include <Rhea/Application.h>
#include <Rhea/UnitTree.h>
#include <Psio/Log.h>

#include <Rhea/EventInput.h>

namespace rhea
{
	bool BasicController::onInput(Event* ev, Unit* unit)
	{
		// safety check
		if (!m_target)
			return false;

		EventInput* e = (EventInput*)ev;

		// default speed in m/s
		float move_step = 1.66f; // ~6km/h 
		float turn_step = 1.3f;
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
		else if (e->isAction("debug")) {
			Unit* tree = m_app->getUnitTree();
			DLOG(LOG_DEBUG) << "Debug positions";
			tree->traverse([](std::string& name, Component* u)->bool {
				std::string type_name = u->getTypeName();
			if ((type_name == "//meworla/item") || (type_name == "//meworla/camera") || (type_name == "//meworla/terrain") || (type_name == "//meworla/character")) {
				DLOG(LOG_DEBUG) << "debugtree: " << doCast<Unit>(u)->getPath();
				math::Vector3 pos = ((Item*)u)->getTransform()->get_position()->getTranslation();
				math::Vector3 pos_d = ((Item*)u)->getTransform()->get_derived_position()->getTranslation();
				DLOG(LOG_DEBUG) << "item:pos(" << pos.x << ", " << pos.y << ", " << pos.z << "); DPos(" << pos_d.x << ", " << pos_d.y << ", " << pos_d.z << ")";
			}
			return true;
				});
		}

		if (m_last_move != m_move) {
			m_last_move = m_move;
			m_target->setMovement(m_move);
		}
		if (m_turn != m_last_turn) {
			m_last_turn = m_turn;
			m_target->setTurn(m_turn);
		}

		return false;
	}
	
	void BasicController::connect(Item* target)
	{
		if (!target) {
			LOG(LOG_ERROR) << "Controller " << m_pathname << ": connect() called with null target";
			return;
		}
		if (m_target && m_target != target) {
			LOG(LOG_WARNING) << "Controller " << m_pathname << ": connect() called with a different target, replacing previous target";
		}
		m_target = target;
	}

	void BasicController::show()
	{
		auto target_it = m_values.find("target");		
		if (target_it != m_values.end()) {
			Item* target = doCast<Item>(m_app->getUnitTree()->find(target_it->second.toString(), m_attributes["basepath"], false));
			if (!target) {
				std::string name = target_it->second.toString();
				LOG(LOG_ERROR) << "Controller " << m_pathname << ": target " << name << " not found";
				return;
			}
			connect(target);
		}
		_registerEvent(m_app->getUnitTree());
	}

	void BasicController::message(std::string method_name)
	{
		Unit::message(method_name);
		if (method_name == "show")
			show();
	}

	void BasicController::setLocalPosition(const Position& position)
	{
		if (!m_target)
			return;

		Position pose = m_target->getTransform()->getWorldPosition();

		pose = position;
		m_target->getTransform()->setLocalPosition(pose);
	}

	void BasicController::setWorldPosition(const Position& position)
	{		
		if (!m_target)
			return;

		Position pose = m_target->getTransform()->getWorldPosition();

		pose = position;

		m_target->getTransform()->setWorldPosition(pose);
	}

	void BasicController::lookAt(math::Vector3& target)
	{
		if (!m_target)
			return;

		Transform* transform = m_target->getTransform();

		Position world_pose = transform->getWorldPosition();
		world_pose.lookAt(target, math::Vector3::UNIT_Z);

		transform->setWorldPosition(world_pose);		
	}

	math::Vector3 BasicController::getWorldPosition()
	{
		if (!m_target)
			return math::Vector3::ZERO;

		return m_target->getTransform()->getWorldPosition().getTranslation();		
	}

	void BasicController::logMe()
	{
		Unit::logMe();
		printf("Target: %s\n", (m_target) ? m_target->getPathName().c_str() : "(empty)");
		if (m_target) {
			math::Vector3 pos = m_target->getTransform()->getWorldPosition().getTranslation();
			printf("Position: (%f,%f,%f)\n", pos.x, pos.y, pos.z);
		}
	}

	void BasicController::_registerEvent(Unit* unittree)
	{
		unittree->registerEventHandler("unhandled_input", this, std::bind(&BasicController::onInput, this, std::placeholders::_1, std::placeholders::_2));
	}
}