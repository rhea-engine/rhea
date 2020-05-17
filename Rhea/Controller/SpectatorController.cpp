#include "SpectatorController.h"
#include "../Item.h"
#include "../Application.h"
#include "../UnitTree.h"
#include "../Camera.h"
#include "../Graphics/Camera.h"

namespace Rhea
{
	void SpectatorController::onInput(std::string action, std::shared_ptr<Psio::InputEvent>& evt)
	{
		// safety check
		if (!m_target)
			return;

		// default speed in m/s
		float move_step = 1.66f; // ~6km/h 
		float turn_step = 0.3f;
		float side_step = 0.575f;
		float up_step = 0.5f;

		// handle actions for the camera (if the camera moves independend of the target)

		if (action == "cam_move_left") {
			//m_move.x = -side_step;
		}
		else if (action == "cam_move_right") {
			//m_move.x = side_step;
		}
		BasicController::onInput(action, evt);
	}

	void SpectatorController::onPhysics(float delta)
	{		
		BasicController::onPhysics(delta);
	}

	void SpectatorController::show(Poseidon::Core* core)
	{

		std::unordered_map<std::string, std::string> attr;
		std::unordered_map<std::string, Variant> values;

		// create the view
		m_view = static_cast<Camera*>(m_app->getUnitTree()->createUnit("scene_view", "//meworla/camera", attr, values));
		GraphicsCamera::createElement(core, m_view);

		// get target
		
		auto target_name_it = m_values.find("target");
		if (target_name_it == m_values.end()) {
			m_app->log("Spectator " + m_pathname + ": no target defined");
			return;
		}
		// connect
		m_target = m_app->getUnitTree()->find(target_name_it->second);
		if (!m_target) {
			std::string name = target_name_it->second;
			m_app->log("Spectator " + m_pathname + ": target " + name + " not found");
			return;
		}		

		// add relative position of view
		auto it = m_values.find("position");
		Poseidon::Vector3 position;
		if (it != m_values.end())
			position = Poseidon::Vector3(it->second[0], it->second[1], it->second[2]);
		else
			position = Poseidon::Vector3(0.0f, 1.7f, -3.0f);
		m_view->setPosition(position);
		m_position.setPosition(position);

		//  add following
		((Rhea::Item*)m_target)->addFollower(m_view);
		m_target->addChild(m_view);
	}

	void SpectatorController::message(std::string method_name)
	{
		Unit::message(method_name);
		if (method_name == "show")
			show(m_app->get_core());
	}

	void SpectatorController::setPosition(Poseidon::Vector3 position)
	{
		m_position.setPosition(position);
		//m_position.setOrientation(Poseidon::Quaternion(m_rotation, Poseidon::Vector3(0.0f, 1.0f, 0.0f))); 
		((Item*)m_target)->setPosition(m_position);

	}

	void SpectatorController::lookAt(Poseidon::Vector3& target)
	{
		m_position.lookAt(target, Poseidon::Vector3::UNIT_Z);
		((Item*)m_target)->setPosition(m_position);
	}

	Unit* SpectatorController::createType(std::string name, std::string type)
	{
		Unit* ret = (Unit*)new SpectatorController(name, type);
		return ret;
	}
}
