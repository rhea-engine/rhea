/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */
#define NOMINMAX
#include <Rhea/Controller/SpectatorController.h>
#include <Rhea/Core/Item.h>
#include <Rhea/Application.h>
#include <Rhea/UnitTree.h>
#include <Rhea/Core/Camera.h>
#include <Rhea/EventInput.h>
#include <Psio/Log.h>
#include <Rhea/Physics/PhysicsService.h>
#include <Rhea/Physics/PhysicsManagerService.h>
#include <algorithm>

namespace rhea
{
	void SpectatorController::setup(Application* app, std::unordered_map<std::string, std::string> attributes, std::unordered_map<std::string, simpleparser::Variant> values)
	{
		BasicController::setup(app, attributes, values);

		m_side_step = getValue("side_step", 0.5f);
	}

	bool SpectatorController::onInput(Event* ev, Unit* unit)
	{
		// safety check
		if (!m_target)
			return false;

		EventInput* e = (EventInput*)ev;
		InputEventType id = e->getEvent().type;

		// handle actions for the camera (if the camera moves independend of the target)

		if (e->isAction("cam_move_left")) {
			m_camera_slide = -m_side_step;
		}
		else if (e->isAction("cam_move_right")) {
			m_camera_slide = m_side_step;
		}
		else if (id == InputEventType::MouseWheel) {
			// mouse wheel zoom
			float wheel = static_cast<float>(e->getEvent().wheelY);

			if (wheel != 0.0f)
			{
				// Positives Mausrad: hineinzoomen
				float direction = wheel > 0.0f ? -1.0f : 1.0f;

				m_zoom_distance = std::clamp(m_zoom_distance + direction * m_zoom_step, m_zoom_min, m_zoom_max);
			}
		}

		//BasicController::onInput(action, evt);
		return BasicController::onInput(ev, unit);
	}

	void SpectatorController::ready()
	{
		BasicController::ready();

		// add relative position of view
		auto it = m_values.find("position");
		if (it != m_values.end())
			m_follow_offset = math::Vector3(it->second[0], it->second[1], it->second[2]);
		else
			m_follow_offset = math::Vector3(0.0f, 1.7f, -3.0f);

		math::Vector3 initial_arm(m_follow_offset.x, 0.0f, m_follow_offset.z);

		float initial_distance = initial_arm.length();
		m_zoom_max = std::max(m_zoom_max, initial_distance);
		m_zoom_min = std::min(m_zoom_min, initial_distance);
		m_zoom_distance = std::clamp(initial_arm.length(), m_zoom_min, m_zoom_max);
	}

	void SpectatorController::setCamera(Camera* camera)
	{
		if (m_view)
			m_view->unregisterEventHandler("delete", this);
		m_view = camera;
		if (m_view)
			m_view->registerEventHandler("delete", this, [this](Event*, Unit*) {
			// targets gets destroyed
			m_view = nullptr;
			return false;
				});
	}

	void SpectatorController::updateCamera(float delta)
	{
		if (!m_target || !m_view)
			return;

		const Position& target_pos = m_target->getTransform()->getWorldPosition();

		math::Vector3 target_world = target_pos.getTranslation();
		math::Quaternion target_rot = target_pos.getRotation();

		Position camera_world_pose;
		math::Vector3 camera_world;
		math::Vector3 camera_pivot;

		// camera zoom
		math::Vector3 arm(m_follow_offset.x, 0.0f, m_follow_offset.z);

		if (arm.length() > 0.0f)
		{
			arm.normalize();
			arm *= m_zoom_distance;
		}

		math::Vector3 local_offset(arm.x + m_camera_slide, m_follow_offset.y, arm.z);

		if (m_follow_rotation) {		
			// Both the follow position and the collision pivot are expressed in
			// target-local space when the camera follows the target rotation.
			camera_world = target_world + target_rot * local_offset;
			camera_pivot = target_world + target_rot * math::Vector3(0.0f, local_offset.y, 0.0f);
		}
		else {			
			// Keep both positions in world space when target rotation is ignored.
			camera_world = target_world + local_offset;
			camera_pivot = target_world + math::Vector3(0.0f, local_offset.y, 0.0f);
		}

		math::Vector3 direction = camera_world - camera_pivot;
		float desired_distance = direction.length();
		if (desired_distance > 0.0f)
		{
			direction.normalize();

			auto* physics_manager = m_app->getService<PhysicsManagerService>();
			auto* physics = physics_manager ? physics_manager->getPhysicsService() : nullptr;
			PhysicsService::PhysicsQueryHit hit;

			if (physics && physics->sphereCast(camera_pivot, camera_world, m_camera_collision_radius, hit, m_target))
			{
				float safe_distance = std::max(0.0f, hit.distance - m_camera_collision_padding);
				camera_world = camera_pivot + direction * safe_distance;
			}
		}

		if (m_follow_rotation) {
			// Change position vector based on target's orientation & scale			
			camera_world_pose.setOrientation(target_rot);
			camera_world_pose.setPosition(camera_world);
		}
		else {
			// Just change position vector based on target's position, ignore rotation
			camera_world_pose.setPosition(camera_world);
			camera_world_pose.lookAt(camera_pivot, math::Vector3::UNIT_Z);
		}
		m_view->setWorldPosition(camera_world_pose);
	}

	void SpectatorController::onFrame(float delta)
	{
		BasicController::onFrame(delta);
		updateCamera(delta);
	}

	void SpectatorController::setPosition(math::Vector3 position)
	{
		if (!m_view)
			return;

		Position pos;
		pos.setPosition(position);
		m_view->setWorldPosition(pos);

	}

	void SpectatorController::lookAt(math::Vector3& target)
	{
		if (!m_view)
			return;

		Position pos = m_view->getTransform()->getWorldPosition();
		pos.lookAt(target, math::Vector3::UNIT_Z);
		m_view->setWorldPosition(pos);
	}
}
