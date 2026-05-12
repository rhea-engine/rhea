/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#include <Rhea/Components/PhysicsDataComponent.h>

namespace rhea
{
	void PhysicsDataComponent::setup(Application* app, std::unordered_map<std::string, std::string> attributes, std::unordered_map<std::string, Variant> values)
	{
		m_app = app;
		if (values.find("enabled") != values.end()) {
			enabled = values["enabled"].toBool();
		}
		if (values.find("motion") != values.end()) {
			std::string motion_str = values["motion"].toString();
			if (motion_str == "static") {
				rigid_body.motion = RigidBody::MotionType::Static;
			}
			else if (motion_str == "dynamic") {
				rigid_body.motion = RigidBody::MotionType::Dynamic;
			}
			else if (motion_str == "kinematic") {
				rigid_body.motion = RigidBody::MotionType::Kinematic;
			}
		}
		if (values.find("mass") != values.end()) {
			rigid_body.mass = values["mass"].toFloat();
		}
		if (rigid_body.mass>0.0f && rigid_body.motion == RigidBody::MotionType::Static) {
			// if mass is set and motion is static, we switch to dynamic
			rigid_body.motion = RigidBody::MotionType::Dynamic;
		}
		if (values.find("gravity") != values.end()) {
			rigid_body.gravity = values["gravity"].toFloat();
		}
		if (values.find("ground_check") != values.end()) {
			rigid_body.ground_check = values["ground_check"].toBool();
		}

		// register as physics data component if not already registered
		if (m_parent && !m_parent->getComponent<PhysicsDataComponent>()) {
			m_parent->addComponent(this);
		}
	}

	void PhysicsDataComponent::ready()
	{
		// get the colliders from my children
		for (const auto& pair : m_children) {
			ColliderComponent* child = doCast<ColliderComponent>(pair.second);
			if (child) {
				colliders.push_back(child);
			}
		}
	}
}
