#include "SpectatorController.h"
#include "Fabrics/3D/Item.h"
#include "../Application.h"
#include "../UnitTree.h"
#include "Fabrics/3D/Camera.h"
#include "../Graphics/CameraManager.h"
#include "../Graphics/GraphicsService.h"
#include "../Graphics/GraphicsManager.h"
#include <Rhea/EventInput.h>
#include <Psio/Log.h>

namespace Rhea
{
	bool SpectatorController::onInput(Event* ev, Unit* unit)
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

		// handle actions for the camera (if the camera moves independend of the target)

		if (e->isAction("cam_move_left")) {
			//m_move.x = -side_step;
		}
		else if (e->isAction("cam_move_right")) {
			//m_move.x = side_step;
		}

		//BasicController::onInput(action, evt);
		return BasicController::onInput(ev, unit);
	}

	void SpectatorController::show()
	{

		std::unordered_map<std::string, std::string> attr;
		std::unordered_map<std::string, Variant> values;

		// create the view
		m_view = static_cast<Camera*>(m_app->getUnitTree()->createUnit("scene_view", "//meworla/camera", attr, values));
		GraphicsService* gs = static_cast<GraphicsService*>(m_app->getService("graphics"));
		GraphicsManager* mgr = gs->getManager("//meworla/camera");
		//GraphicsCamera::createElement(core, m_view);
		mgr->createElement(gs->getEngine(), m_view);

		// get target
		
		auto target_name_it = m_values.find("target");
		if (target_name_it == m_values.end()) {
			LOG(LOG_ERROR) << "Spectator " << m_pathname << ": no target defined";
			return;
		}
		// connect
		m_target = static_cast<Item*>(m_app->getUnitTree()->find(target_name_it->second));
		if (!m_target) {
			std::string name = target_name_it->second;
			LOG(LOG_ERROR) << "Spectator " << m_pathname << ": target " << name << " not found";
			return;
		}		

		// add relative position of view
		auto it = m_values.find("position");
		Math::Vector3 position;
		if (it != m_values.end())
			position = Math::Vector3(it->second[0], it->second[1], it->second[2]);
		else
			position = Math::Vector3(0.0f, 1.7f, -3.0f);
		m_view->setPosition(position);
		m_position.setPosition(position);

		//  add following
		m_target->addChild(m_view);

		_registerEvent(m_app->getUnitTree());
	}

	void SpectatorController::message(std::string method_name)
	{
		Unit::message(method_name);
		if (method_name == "show")
			show();
	}

	void SpectatorController::setPosition(Math::Vector3 position)
	{
		m_position.setPosition(position);
		//m_position.setOrientation(Math::Quaternion(m_rotation, Math::Vector3(0.0f, 1.0f, 0.0f))); 
		((Item*)m_target)->setPosition(m_position);

	}

	void SpectatorController::lookAt(Math::Vector3& target)
	{
		m_position.lookAt(target, Math::Vector3::UNIT_Z);
		((Item*)m_target)->setPosition(m_position);
	}

	void SpectatorController::_registerEvent(Unit* unittree)
	{
		unittree->registerEventHandler("unhandled_input", this, std::bind(&SpectatorController::onInput, this, std::placeholders::_1, std::placeholders::_2));
	}

	Unit* SpectatorController::createType(std::string name, std::string type)
	{
		Unit* ret = (Unit*)new SpectatorController(name, type);
		return ret;
	}
}
