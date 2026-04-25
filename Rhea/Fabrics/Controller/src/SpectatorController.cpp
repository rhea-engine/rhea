#include <Rhea/Controller/SpectatorController.h>
#include <Rhea/Core/Item.h>
#include <Rhea/Application.h>
#include <Rhea/UnitTree.h>
#include <Rhea/Core/Camera.h>
//#include <Rhea/Graphics/CameraManager.h>
//#include <Rhea/Graphics/GraphicsService.h>
//#include <Rhea/Graphics/GraphicsManager.h>
#include <Rhea/EventInput.h>
#include <Psio/Log.h>

namespace rhea
{
	bool SpectatorController::onInput(Event* ev, Unit* unit)
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

		// handle actions for the camera (if the camera moves independend of the target)

		if (e->isAction("cam_move_left")) {
			m_camera_slide = -side_step;
		}
		else if (e->isAction("cam_move_right")) {
			m_camera_slide = side_step;
		}

		//BasicController::onInput(action, evt);
		return BasicController::onInput(ev, unit);
	}

	void SpectatorController::show()
	{
		BasicController::show();

		if (!m_target) {
			LOG(LOG_ERROR) << "Spectator " << m_pathname << ": no target defined";
			return;
		}

		std::unordered_map<std::string, std::string> attr;
		std::unordered_map<std::string, Variant> values;

		// create the view		
		m_view = m_app->getUnitTree()->createUnit<Camera>("scene_view", attr, values);	

		// add relative position of view
		auto it = m_values.find("position");
		if (it != m_values.end())
			m_follow_offset = math::Vector3(it->second[0], it->second[1], it->second[2]);
		else
			m_follow_offset = math::Vector3(0.0f, 1.7f, -3.0f);

		m_position.setPosition(m_follow_offset);
		m_view->setPosition(m_follow_offset);
		m_view->getTransform()->markDirty();	
		addChild(m_view);

		// Fire scene_enter event to notify Scene to add graphics components to the view
		Event enter_event("scene_enter", m_view);
		m_view->bubbleEvent("scene_enter", &enter_event); m_target->bubbleEvent("scene_enter", &enter_event);

		_registerEvent(m_app->getUnitTree());
	}

	void SpectatorController::updateCamera(float delta)
	{
		if (!m_target || !m_view)
			return;

		auto* target_pos = m_target->getTransform()->get_derived_position();

		math::Vector3 target_world = target_pos->get_worldspace();
		math::Quaternion target_rot = target_pos->get_rotation();

		Position camera_pos;
		math::Vector3 camera_world;

		math::Vector3 local_offset = m_follow_offset;
		local_offset.x += m_camera_slide;

		if (m_follow_rotation) {		
			// Change position vector based on target's orientation & scale
			camera_world = target_world + target_rot * local_offset;
			camera_pos.setOrientation(target_rot);
			camera_pos.setPosition(camera_world);
		}
		else {			
			// Just change position vector based on target's position, ignore rotation
			camera_world = target_world + target_rot * local_offset;
			camera_pos.setPosition(camera_world);
			camera_pos.lookAt(target_world, math::Vector3::UNIT_Z);
		}
		
		m_view->setPosition(camera_pos);
	}

	void SpectatorController::onFrame(float delta)
	{
		BasicController::onFrame(delta);
		updateCamera(delta);
		return;
		auto targetPos = m_target->getTransform()
			->get_derived_position()
			->get_worldspace();

		Position camPos;
		camPos.setPosition(targetPos + m_follow_offset);

		m_view->setPosition(camPos);
	}

	void SpectatorController::message(std::string method_name)
	{
		Unit::message(method_name);
		if (method_name == "show")
			show();
	}

	void SpectatorController::setPosition(math::Vector3 position)
	{
		if (!m_view)
			return;

		Position pos;
		pos.setPosition(position);
		m_view->setPosition(pos);

	}

	void SpectatorController::lookAt(math::Vector3& target)
	{
		if (!m_view)
			return;

		Position pos = *m_view->getTransform()->get_derived_position();
		pos.lookAt(target, math::Vector3::UNIT_Z);
		m_view->setPosition(pos);
	}

	void SpectatorController::_registerEvent(Unit* unittree)
	{
		unittree->registerEventHandler("unhandled_input", this, std::bind(&SpectatorController::onInput, this, std::placeholders::_1, std::placeholders::_2));
	}
}
